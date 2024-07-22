#pragma once

#include <cooperative_groups.h>
// #include <cooperative_groups/reduce.h>
namespace cg = cooperative_groups;

// Utility class used to avoid linker errors with extern
// unsized shared memory arrays with templated type
template <class T>
struct SharedMemory {
    __device__ inline operator T* () {
        extern __shared__ int __smem[];
        return (T*)__smem;
    }

    __device__ inline operator const T* () const {
        extern __shared__ int __smem[];
        return (T*)__smem;
    }
};

// specialize for double to avoid unaligned memory
// access compile errors
template <>
struct SharedMemory<double> {
    __device__ inline operator double* () {
        extern __shared__ double __smem_d[];
        return (double*)__smem_d;
    }

    __device__ inline operator const double* () const {
        extern __shared__ double __smem_d[];
        return (double*)__smem_d;
    }
};

template <class T>
__device__ __forceinline__ T warpReduceSum(unsigned int mask, T mySum) {
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        mySum += __shfl_down_sync(mask, mySum, offset);
    }
    return mySum;
}

#if __CUDA_ARCH__ >= 800
// Specialize warpReduceFunc for int inputs to use __reduce_add_sync intrinsic
// when on SM 8.0 or higher
template <>
__device__ __forceinline__ int warpReduceSum<int>(unsigned int mask,
    int mySum) {
    mySum = __reduce_add_sync(mask, mySum);
    return mySum;
}
#endif

inline bool isPow2(unsigned int x)
{
    return ((x & (x - 1)) == 0);
}

// template <typename T, typename Group>
// __device__ T cg_reduce_n(T in, Group& threads) {
//     return cg::reduce(threads, in, cg::plus<T>());
// }

// template <typename T, typename Group>
// __device__ void reduce(
//         T& mySum,
//         T* sdata,
//         unsigned tid,
//         cg::thread_block& cta,
//         Group& tile32,
//         int blockSize) {
//     // each thread puts its local sum into shared memory
//     sdata[tid] = mySum;
//     cg::sync(cta);

//     // do reduction in shared mem
//     if ((blockSize >= 1024) && (tid < 512)) {
//         sdata[tid] = mySum = mySum + sdata[tid + 512];
//     }
//     cg::sync(cta);

//     if ((blockSize >= 512) && (tid < 256)) {
//         sdata[tid] = mySum = mySum + sdata[tid + 256];
//     }

//     cg::sync(cta);

//     if ((blockSize >= 256) && (tid < 128)) {
//         sdata[tid] = mySum = mySum + sdata[tid + 128];
//     }
//     cg::sync(cta);

//     if ((blockSize >= 128) && (tid < 64)) {
//         sdata[tid] = mySum = mySum + sdata[tid + 64];
//     }
//     cg::sync(cta);

//     if ((blockSize >= 64) && (tid < 32)) {
//         mySum += sdata[tid + 32];
//     }
//     cg::sync(cta);
// }
