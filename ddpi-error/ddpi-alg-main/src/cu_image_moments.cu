#include "cu_helper.cuh"
#include "cu_image_moments.cuh"

template<typename InputT, typename OutputT>
__device__ void imageMoments(
        InputT* pSrc,
        OutputT& m1, OutputT& m2, OutputT& m3,
        int idx,
        int width, int height, int step,
        OutputT threshold) {
    OutputT x = static_cast<OutputT>(idx % step);
    OutputT y = static_cast<OutputT>(idx / step);
    if ((x < static_cast<OutputT>(width)) & (y < static_cast<OutputT>(height))) {
        OutputT val = static_cast<OutputT>(pSrc[idx]);
        if (val < threshold) {
            val = 0;
        }
        m1 += val;
        m2 += x* val;
        m3 += y* val;
    }
}

template<typename InputT, typename OutputT>
__device__ void imageMomentsPower(
        InputT* pSrc,
        OutputT& m1, OutputT& m2, OutputT& m3,
        int idx,
        int width, int height, int step,
        OutputT power) {
    OutputT x = static_cast<OutputT>(idx % step);
    OutputT y = static_cast<OutputT>(idx / step);
    if ((x < static_cast<OutputT>(width)) & (y < static_cast<OutputT>(height))) {
        OutputT val = pow(static_cast<OutputT>(pSrc[idx]), power);
        m1 += val;
        m2 += x * val;
        m3 += y * val;
    }
}

template<class InputT, class OutputT, bool nIsPow2>
__global__ void imageMomentsKernel(
        InputT *pSrc, int step,
        OutputT *pDst,
        int width, int height,
        OutputT threshold) {

    // Shared memory for intermediate steps
    OutputT* sdata = SharedMemory<OutputT>();

    // Handle to thread block group
    cg::thread_block cta = cg::this_thread_block();

    // Handle to tile in thread block
    cg::thread_block_tile<32> tile = cg::tiled_partition<32>(cta);
    unsigned int n = step * height;

    unsigned int threadRank = threadIdx.x;
    unsigned int blocksize = cta.size();
    OutputT threadVal00 = 0;
    OutputT threadVal10 = 0;
    OutputT threadVal01 = 0;

    if (nIsPow2) {
        unsigned int i = blockIdx.x * blocksize * 2 + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x << 1;
        while (i < n) {
            imageMoments(
                    pSrc,
                    threadVal00, threadVal10, threadVal01,
                    i,
                    width, height, step,
                    threshold);
            // ensure we don't read out of bounds -- this is optimized away for
            // powerOf2 sized arrays
            int j = i + blocksize;
            if (j < n) {
                imageMoments(
                        pSrc,
                        threadVal00, threadVal10, threadVal01,
                        j,
                        width, height, step,
                        threshold);
            }
            i += gridSize;
        }
    }
    else {
        unsigned int i = blockIdx.x * blocksize + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x;
        while (i < n) {
            imageMoments(
                    pSrc,
                    threadVal00, threadVal10, threadVal01,
                    i,
                    width, height, step,
                    threshold);
            i += gridSize;
        }
    }

    sdata[threadRank] = threadVal00;
    sdata[threadRank + blocksize] = threadVal10;
    sdata[threadRank + 2 * blocksize] = threadVal01;
    cg::sync(cta);

    // do reduction in shared mem
    if (threadRank < 512) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 512];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 512];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 512];
    }
    cg::sync(cta);

    if (threadRank < 256) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 256];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 256];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 256];
    }
    cg::sync(cta);

    if (threadRank < 128) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 128];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 128];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 128];
    }
    cg::sync(cta);

    if (threadRank < 64) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 64];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 64];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 64];
    }
    cg::sync(cta);

    if (threadRank < 32) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 32];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 32];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 32];
    }
    cg::sync(cta);

    //reduce(threadVal00, sdata, threadRank, cta, tile, 1024);
    //reduce(threadVal10, sdata, threadRank, cta, tile, 1024);
    //reduce(threadVal01, sdata, threadRank, cta, tile, 1024);

    if (threadRank < 32) {
        // Reduce final warp using shuffle
        for (int offset = tile.size() / 2; offset > 0; offset /= 2) {
            threadVal00 += tile.shfl_down(threadVal00, offset);
            threadVal01 += tile.shfl_down(threadVal01, offset);
            threadVal10 += tile.shfl_down(threadVal10, offset);
        }
    }

    if (threadRank == 0) {
        atomicAdd(pDst, threadVal00);
        atomicAdd(pDst + 1, threadVal10);
        atomicAdd(pDst + 2, threadVal01);
    }
}

template<class InputT, class OutputT, bool nIsPow2>
__global__ void imagePowerMomentsKernel(
        InputT *pSrc, int step,
        OutputT *pDst,
        int width, int height,
        OutputT power) {
    // Shared memory for intermediate steps
    OutputT* sdata = SharedMemory<OutputT>();

    // Handle to thread block group
    cg::thread_block cta = cg::this_thread_block();

    // Handle to tile in thread block
    cg::thread_block_tile<32> tile = cg::tiled_partition<32>(cta);
    unsigned int n = step * height;

    unsigned int threadRank = threadIdx.x;
    unsigned int blocksize = cta.size();
    OutputT threadVal00 = 0;
    OutputT threadVal10 = 0;
    OutputT threadVal01 = 0;

    if (nIsPow2) {
        unsigned int i = blockIdx.x * blocksize * 2 + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x << 1;
        while (i < n) {
            imageMomentsPower(
                    pSrc,
                    threadVal00, threadVal10, threadVal01,
                    i,
                    width, height, step,
                    power);
            // ensure we don't read out of bounds -- this is optimized away for
            // powerOf2 sized arrays
            int j = i + blocksize;
            if (j < n) {
                imageMomentsPower(
                        pSrc,
                        threadVal00, threadVal10, threadVal01,
                        j,
                        width, height, step,
                        power);
            }
            i += gridSize;
        }
    }
    else {
        unsigned int i = blockIdx.x * blocksize + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x;
        while (i < n) {
            imageMomentsPower(
                    pSrc,
                    threadVal00, threadVal10, threadVal01,
                    i,
                    width, height, step,
                    power);
            i += gridSize;
        }
    }

    sdata[threadRank] = threadVal00;
    sdata[threadRank + blocksize] = threadVal10;
    sdata[threadRank + 2 * blocksize] = threadVal01;
    cg::sync(cta);

    // do reduction in shared mem
    if (threadRank < 512) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 512];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 512];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 512];
    }
    cg::sync(cta);

    if (threadRank < 256) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 256];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 256];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 256];
    }
    cg::sync(cta);

    if (threadRank < 128) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 128];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 128];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 128];
    }
    cg::sync(cta);

    if (threadRank < 64) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 64];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 64];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 64];
    }
    cg::sync(cta);

    if (threadRank < 32) {
        sdata[threadRank] = threadVal00 = threadVal00 + sdata[threadRank + 32];
        sdata[threadRank + blocksize] = threadVal10 = threadVal10 + sdata[threadRank + blocksize + 32];
        sdata[threadRank + 2 * blocksize] = threadVal01 = threadVal01 + sdata[threadRank + 2 * blocksize + 32];

        // Reduce final warp using shuffle
        for (int offset = tile.size() / 2; offset > 0; offset /= 2) {
            threadVal00 += tile.shfl_down(threadVal00, offset);
            threadVal01 += tile.shfl_down(threadVal01, offset);
            threadVal10 += tile.shfl_down(threadVal10, offset);
        }
    }
    cg::sync(cta);

    if (threadRank == 0) {
        atomicAdd(pDst, threadVal00);
        atomicAdd(pDst + 1, threadVal10);
        atomicAdd(pDst + 2, threadVal01);
    }
}

template<class InputT, class OutputT>
cudaError_t imageMomentsImpl(
        InputT *pSrc, int step,
        OutputT *pDst,
        int width, int height,
        OutputT threshold,
        int blocks, int threads,
        cudaStream_t& ctx) {

    cudaError_t err = cudaMemsetAsync(pDst, 0, 3 * sizeof(OutputT), ctx);
    if (err != cudaSuccess) return err;

    err = cudaStreamSynchronize(ctx);
    if (err != cudaSuccess) return err;

    dim3 dimBlock(threads, 1, 1);
    dim3 dimGrid(blocks, 1, 1);
    int smemSize = (threads <= 32) ? 6 * threads * sizeof(OutputT) : 3 * threads * sizeof(OutputT);

    if (isPow2(height * step)) {
        imageMomentsKernel<InputT, OutputT, true> << <dimGrid, dimBlock, smemSize, ctx >> > (
                pSrc, step, pDst, width, height, threshold);
    }
    else {
        imageMomentsKernel<InputT, OutputT, false> << <dimGrid, dimBlock, smemSize, ctx >> > (
                pSrc, step, pDst, width, height, threshold);
    }


    err = cudaGetLastError();
    if (err != cudaSuccess) return err;

    err = cudaStreamSynchronize(ctx);
    return err;
}

template<class InputT, class OutputT>
cudaError_t imagePowerMomentsImpl(
        InputT *pSrc, int step,
        OutputT *pDst,
        int width, int height,
        OutputT power,
        int blocks, int threads,
        cudaStream_t& ctx) {

    cudaError_t err = cudaMemsetAsync(pDst, 0, 3 * sizeof(OutputT), ctx);
    if (err != cudaSuccess) return err;

    err = cudaStreamSynchronize(ctx);
    if (err != cudaSuccess) return err;

    dim3 dimBlock(threads, 1, 1);
    dim3 dimGrid(blocks, 1, 1);
    int smemSize = (threads <= 32) ? 6 * threads * sizeof(OutputT) : 3 * threads * sizeof(OutputT);
    if (isPow2(height * step)) {
        imagePowerMomentsKernel<InputT, OutputT, true> << <dimGrid, dimBlock, smemSize, ctx >> > (
                pSrc, step, pDst, width, height, power);
    }
    else {
        imagePowerMomentsKernel<InputT, OutputT, false> << <dimGrid, dimBlock, smemSize, ctx >> > (
                pSrc, step, pDst, width, height, power);
    }
    err = cudaGetLastError();
    if (err != cudaSuccess) return err;

    err = cudaStreamSynchronize(ctx);
    return err;
}

cudaError_t imageMoments_8u32u(
        unsigned char* pSrc, int step,
        unsigned int *pDst,
        int width, int height,
        unsigned int threshold,
        int blocks, int threads,
        cudaStream_t &ctx) {
    return imageMomentsImpl<unsigned char, unsigned int>(
            pSrc, step, pDst, width, height, threshold, blocks, threads, ctx);
}

cudaError_t imageMoments_32f(
        float *pSrc, int step,
        float *pDst,
        int width, int height,
        float threshold,
        int blocks, int threads,
        cudaStream_t &ctx) {
    return imageMomentsImpl<float, float>(
            pSrc, step, pDst, width, height, threshold, blocks, threads, ctx);
}

cudaError_t imageMoments_32f64f(
        float *pSrc, int step,
        double *pDst,
        int width, int height,
        double threshold,
        int blocks, int threads,
        cudaStream_t &ctx) {
    return imageMomentsImpl<float, double>(
            pSrc, step, pDst, width, height, threshold, blocks, threads, ctx);

}

cudaError_t imagePowerMoments_32f(
        float* pSrc, int step,
        float* pDst,
        int width, int height,
        float power,
        int blocks, int threads,
        cudaStream_t& ctx) {
    return imagePowerMomentsImpl<float, float>(
            pSrc, step, pDst, width, height, power, blocks, threads, ctx);
}
