#pragma once

cudaError_t imageMoments_8u32u(
        unsigned char* pSrc, int step,
        unsigned int *pDst,
        int width, int height,
        unsigned int threshold,
        int blocks, int threads,
        cudaStream_t &ctx);

cudaError_t imageMoments_32f(
        float *pSrc, int step,
        float *pDst,
        int width, int height,
        float threshold,
        int blocks, int threads,
        cudaStream_t &ctx);

cudaError_t imagePowerMoments_32f(
        float *pSrc, int step,
        float *pDst,
        int width, int height,
        float power,
        int blocks, int threads,
        cudaStream_t &ctx);

cudaError_t imageMoments_32f64f(
        float *pSrc, int step,
        double *pDst,
        int width, int height,
        double threshold,
        int blocks, int threads,
        cudaStream_t &ctx);
