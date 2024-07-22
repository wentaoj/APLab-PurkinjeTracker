#pragma once
#include <driver_types.h>

#define CUDA_MAX_THREAD 1024
#define CUDA_MAX_BLOCKSIZE 64

#include "gpu_array.hpp"
#include "gpu_2darray.hpp"


namespace ddpi {

unsigned int nextPow2(unsigned int x);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
bool getNumBlocksAndThreadsForReduction(
        const cudaDeviceProp &prop,
        GpuArray <T> &data) {
    int n = data.size();
    int threads = (n < CUDA_MAX_THREAD * 2) ? nextPow2((n + 1) / 2) : CUDA_MAX_THREAD;
    int blocks = (n + (threads * 2 - 1)) / (threads * 2);

    if ((float) threads * blocks > (float) prop.maxGridSize[0] * prop.maxThreadsPerBlock) {
        return false;
    }

    if (blocks > prop.maxGridSize[0]) {
        blocks /= 2;
        threads *= 2;
    }

    blocks = (CUDA_MAX_BLOCKSIZE < blocks) ? CUDA_MAX_BLOCKSIZE : blocks;

    data.setBlocksAndThreads(blocks, threads);

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
bool getNumBlocksAndThreadsForReduction(
        const cudaDeviceProp &prop,
        Gpu2DArray <T> &data) {
    //
    int n = data.paddedWidth() * data.height();
    int threads = (n < CUDA_MAX_THREAD * 2) ? nextPow2((n + 1) / 2) : CUDA_MAX_THREAD;
    int blocks = (n + (threads * 2 - 1)) / (threads * 2);

    if ((float) threads * blocks > (float) prop.maxGridSize[0] * prop.maxThreadsPerBlock) {
        return false;
    }

    if (blocks > prop.maxGridSize[0]) {
        blocks /= 2;
        threads *= 2;
    }

    blocks = (CUDA_MAX_BLOCKSIZE < blocks) ? CUDA_MAX_BLOCKSIZE : blocks;

    data.setBlocksAndThreads(blocks, threads);
    return true;
}

}