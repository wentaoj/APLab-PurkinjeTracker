#pragma once
#include "cu_gaussian.cuh"

int cuOverlayRect(
        unsigned char *dst, int step,
        int width, int height,
        int xc, int yc, int w, int h,
        unsigned char value,
        cudaStream_t& ctx);

int cuOverlay(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char value,
        cudaStream_t& ctx);