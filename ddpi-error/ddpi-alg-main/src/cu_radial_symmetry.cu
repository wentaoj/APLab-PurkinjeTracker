#include "cu_helper.cuh"
#include "cu_image_moments.cuh"
#include "cu_radial_symmetry.cuh"

template<typename T, typename InputT, typename OutputT>
__device__ void radialSymmetry(
        InputT* DX, InputT* DY, InputT* DI, InputT* angle, InputT* I,
        OutputT& W, OutputT& BW, OutputT& MW, OutputT& MMW, OutputT& MBW, OutputT& R2,
        int idx,
        int width, int height, int step,
        T cx, T cy,
        int threshold) {
    OutputT x = static_cast<OutputT>(idx % step);
    OutputT y = static_cast<OutputT>(idx / step);
    if ((x < static_cast<OutputT>(width)) & (y < static_cast<OutputT>(height))) {
        OutputT dx = static_cast<OutputT>(DX[idx]);
        OutputT dy = static_cast<OutputT>(DY[idx]);
        OutputT m = 0;
        OutputT b = 0;
        OutputT w = 0;
        OutputT r2 = 0;
        OutputT dI2 = 0;
        OutputT Mag = 0;
        OutputT power2 = static_cast<OutputT>(2);
        if (dx == 0) {
            if (dy == 0) m = static_cast<OutputT>(tan(angle[idx]));
            else m = static_cast<OutputT>(1000000000000);
        }
        else {
            m = dy / dx;
        }

        Mag = static_cast<OutputT>(I[idx]);
        if (Mag > static_cast<OutputT>(threshold)) {
            dI2 = pow(static_cast<OutputT>(DI[idx]), power2);
        }
        else {
            dI2 = 0;
            Mag = 0;
        }

        b = (static_cast<OutputT>(y) - m * static_cast<OutputT>(x));
        r2 = pow(x - static_cast<OutputT>(cx), power2) + pow(y - static_cast<OutputT>(cy), power2);
        w = dI2 / sqrt(r2) / (pow(m, power2) + 1);

        W += w;
        BW += b * w;
        MW += m * w;
        MMW += m * m * w;
        MBW += m * b * w;
        R2 += r2;
    }
}

template<class T, class OutputT, bool nIsPow2>
__global__ void radialSymmetryKernel(
        T* DX,
        T* DY,
        T* DI,
        T* angle,
        T* I,
        int step,
        int width, int height,
        T cx, T cy,
        T threshold,
        OutputT *pDst) {

    // Shared memory for intermediate steps
    OutputT* sdata = SharedMemory<OutputT>();

    // Handle to thread block group
    cg::thread_block cta = cg::this_thread_block();

    // Handle to tile in thread block
    cg::thread_block_tile<32> tile = cg::tiled_partition<32>(cta);
    unsigned int n = step * height;
    unsigned int threadRank = threadIdx.x;
    unsigned int blocksize = cta.size();

    OutputT W = 0;
    OutputT BW = 0;
    OutputT MW = 0;
    OutputT MMW = 0;
    OutputT MBW = 0;
    OutputT R2 = 0;

    if (nIsPow2) {
        unsigned int i = blockIdx.x * blocksize * 2 + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x << 1;
        while (i < n) {
            radialSymmetry(
                    DX, DY, DI, angle, I,
                    W, BW, MW, MMW, MBW, R2,
                    i,
                    width, height, step,
                    cx, cy,
                    threshold);

            int j = i + blocksize;
            if (j < n) {
                radialSymmetry(
                        DX, DY, DI, angle, I,
                        W, BW, MW, MMW, MBW, R2,
                        j,
                        width, height, step,
                        cx, cy,
                        threshold);
            }
            i += gridSize;
        }
    }
    else {
        unsigned int i = blockIdx.x * blocksize + threadIdx.x;
        unsigned int gridSize = blocksize * gridDim.x;
        while (i < n) {
            radialSymmetry(
                    DX, DY, DI, angle, I,
                    W, BW, MW, MMW, MBW, R2,
                    i,
                    width, height, step,
                    cx, cy,
                    threshold);
            i += gridSize;
        }
    }

    //reduce(W, sdata, threadRank, cta, tile, 1024);
    //reduce(BW, sdata, threadRank, cta, tile, 1024);
    //reduce(MW, sdata, threadRank, cta, tile, 1024);
    //reduce(MMW, sdata, threadRank, cta, tile, 1024);
    //reduce(MBW, sdata, threadRank, cta, tile, 1024);
    //reduce(R2, sdata, threadRank, cta, tile, 1024);

    sdata[threadRank] = W;
    sdata[threadRank + blocksize] = BW;
    sdata[threadRank + 2 * blocksize] = MW;
    sdata[threadRank + 3 * blocksize] = MMW;
    sdata[threadRank + 4 * blocksize] = MBW;
    sdata[threadRank + 5 * blocksize] = R2;
    cg::sync(cta);

    // do reduction in shared mem
    if ((blocksize >=1024) & (threadRank < 512)) {
        sdata[threadRank] = W = W + sdata[threadRank + 512];
        sdata[threadRank + blocksize] = BW = BW + sdata[threadRank + blocksize + 512];
        sdata[threadRank + 2 * blocksize] = MW = MW + sdata[threadRank + 2 * blocksize + 512];
        sdata[threadRank + 3 * blocksize] = MMW = MMW + sdata[threadRank + 3 * blocksize + 512];
        sdata[threadRank + 4 * blocksize] = MBW = MBW + sdata[threadRank + 4 * blocksize + 512];
        sdata[threadRank + 5 * blocksize] = R2 = R2 + sdata[threadRank + 5 * blocksize + 512];
    }
    cg::sync(cta);

    if ((blocksize >= 512) & (threadRank < 256)) {
        sdata[threadRank] = W = W + sdata[threadRank + 256];
        sdata[threadRank + blocksize] = BW = BW + sdata[threadRank + blocksize + 256];
        sdata[threadRank + 2 * blocksize] = MW = MW + sdata[threadRank + 2 * blocksize + 256];
        sdata[threadRank + 3 * blocksize] = MMW = MMW + sdata[threadRank + 3 * blocksize + 256];
        sdata[threadRank + 4 * blocksize] = MBW = MBW + sdata[threadRank + 4 * blocksize + 256];
        sdata[threadRank + 5 * blocksize] = R2 = R2 + sdata[threadRank + 5 * blocksize + 256];
    }
    cg::sync(cta);

    if ((blocksize >= 256) & (threadRank < 128)) {
        sdata[threadRank] = W = W + sdata[threadRank + 128];
        sdata[threadRank + blocksize] = BW = BW + sdata[threadRank + blocksize + 128];
        sdata[threadRank + 2 * blocksize] = MW = MW + sdata[threadRank + 2 * blocksize + 128];
        sdata[threadRank + 3 * blocksize] = MMW = MMW + sdata[threadRank + 3 * blocksize + 128];
        sdata[threadRank + 4 * blocksize] = MBW = MBW + sdata[threadRank + 4 * blocksize + 128];
        sdata[threadRank + 5 * blocksize] = R2 = R2 + sdata[threadRank + 5 * blocksize + 128];
    }
    cg::sync(cta);

    if ((blocksize >= 128) & (threadRank < 64)) {
        sdata[threadRank] = W = W + sdata[threadRank + 64];
        sdata[threadRank + blocksize] = BW = BW + sdata[threadRank + blocksize + 64];
        sdata[threadRank + 2 * blocksize] = MW = MW + sdata[threadRank + 2 * blocksize + 64];
        sdata[threadRank + 3 * blocksize] = MMW = MMW + sdata[threadRank + 3 *  blocksize + 64];
        sdata[threadRank + 4 * blocksize] = MBW = MBW + sdata[threadRank + 4 * blocksize + 64];
        sdata[threadRank + 5 * blocksize] = R2 = R2 + sdata[threadRank + 5 * blocksize + 64];
    }
    cg::sync(cta);

    if ((blocksize >= 64) & (threadRank < 32)) {
        sdata[threadRank] = W = W + sdata[threadRank + 32];
        sdata[threadRank + blocksize] = BW = BW + sdata[threadRank + blocksize + 32];
        sdata[threadRank + 2 * blocksize] = MW = MW + sdata[threadRank + 2 * blocksize + 32];
        sdata[threadRank + 3 * blocksize] = MMW = MMW + sdata[threadRank + 3 * blocksize + 32];
        sdata[threadRank + 4 * blocksize] = MBW = MBW + sdata[threadRank + 4 * blocksize + 32];
        sdata[threadRank + 5 * blocksize] = R2 = R2 + sdata[threadRank + 5 * blocksize + 32];

        // Reduce final warp using shuffle
        for (int offset = tile.size() / 2; offset > 0; offset /= 2) {
            W += tile.shfl_down(W, offset);
            BW += tile.shfl_down(BW, offset);
            MW += tile.shfl_down(MW, offset);
            MMW += tile.shfl_down(MMW, offset);
            MBW += tile.shfl_down(MBW, offset);
            R2 += tile.shfl_down(R2, offset);
        }
    }
    cg::sync(cta);

    if (threadRank == 0) {
        atomicAdd(pDst, W);
        atomicAdd(pDst + 1, BW);
        atomicAdd(pDst + 2, MW);
        atomicAdd(pDst + 3, MBW);
        atomicAdd(pDst + 4, MMW);
        atomicAdd(pDst + 5, R2);
    }
}

template<class T, class OutputT>
cudaError_t radialSymmetryImpl(
        T* DX,
        T* DY,
        T* DI,
        T* angle,
        T* I,
        int step,
        int width, int height,
        T cx, T cy,
        T threshold,
        OutputT *pDst,
        int blocks, int threads,
        cudaStream_t& ctx) {
    cudaError_t err = cudaMemset(pDst, 0, 6 * sizeof(T));
    if (err != cudaSuccess) return err;

//    if (threads >= 1024) {
//        blocks = blocks * threads / 512;
//        threads = 512;
//    }

    dim3 dimBlock(threads, 1, 1);
    dim3 dimGrid(blocks, 1, 1);
    int smemSize = (threads <= 32) ? 12 * threads * sizeof(OutputT) : 6 * threads * sizeof(OutputT);

    if (isPow2(height * step)) {
        radialSymmetryKernel<T, OutputT, true> << <dimGrid, dimBlock, smemSize, ctx >> > (
                DX, DY, DI, angle,
                        I,
                        step,
                        width, height,
                        cx, cy,
                        threshold,
                        pDst);
    }
    else {
        radialSymmetryKernel<T, OutputT, false> << <dimGrid, dimBlock, smemSize, ctx >> > (
                DX, DY, DI, angle,
                        I,
                        step,
                        width, height,
                        cx, cy,
                        threshold,
                        pDst);
    }

    err = cudaGetLastError();
    if (err != cudaSuccess) return err;

    err = cudaStreamSynchronize(ctx);
    return err;
}

cudaError_t radialSymmetry_32f(
        float* DX,
        float* DY,
        float* DI,
        float* angle,
        float* I,
        int step,
        int width, int height,
        float cx, float cy,
        float threshold,
        float *pDst,
        int blocks, int threads,
        cudaStream_t &ctx) {
    return radialSymmetryImpl<float, float>(
            DX, DY, I, angle,
            I,
            step,
            width, height,
            cx, cy,
            threshold,
            pDst,
            blocks, threads,
            ctx);
}

cudaError_t radialSymmetry_64f(
        float* DX,
        float* DY,
        float* DI,
        float* angle,
        float* I,
        int step,
        int width, int height,
        float cx, float cy,
        float threshold,
        double *pDst,
        int blocks, int threads,
        cudaStream_t &ctx) {
    return radialSymmetryImpl<float, double>(
            DX, DY, DI, angle,
            I,
            step,
            width, height,
            cx, cy,
            threshold,
            pDst,
            blocks, threads,
            ctx);
}
