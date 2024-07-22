#pragma once
#include "cu_gaussian.cuh"

int cuDrawCircle(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char R, unsigned char G, unsigned char B,
        float lw,
        cudaStream_t& ctx);

int cuDrawRect(
        unsigned char *dst, int step,
        int width, int height,
        int xc, int yc, int w, int h,
        unsigned char R, unsigned char G, unsigned char B,
        unsigned char lw,
        cudaStream_t& ctx);