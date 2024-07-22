#pragma once

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
        cudaStream_t &ctx);

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
        cudaStream_t &ctx);

