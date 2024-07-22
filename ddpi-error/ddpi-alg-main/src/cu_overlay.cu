#include <driver_types.h>
#include "cu_helper.cuh"
#include "cu_overlay.cuh"
#include <algorithm>

__global__ void cuOverlayKernel(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char value) {
    int tx = threadIdx.x + blockIdx.x * blockDim.x;
    int ty = threadIdx.y + blockIdx.y * blockDim.y;

    if ((tx > width) | (ty > height)) {
        return;
    }

    float dx = ((float)tx - xc);
    float dy = ((float)ty - yc);
    float d = sqrtf(dx * dx + dy * dy);
    if (d < r) {
        dst[tx + ty * step] = value;
    }
}

__global__ void cuOverlayRectKernel(
        unsigned char *dst, int step,
        int width, int height,
        int x, int y,
        int xend, int yend,
        unsigned char value) {
    int tx = threadIdx.x + blockIdx.x * blockDim.x;
    int ty = threadIdx.y + blockIdx.y * blockDim.y;

    if ((tx >= x) & (ty >= y) &
        (tx < xend) & (ty < yend)) {
        dst[tx + ty * step] = value;
    }
}

int cuOverlay(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char value,
        cudaStream_t& ctx) {
	dim3 dimBlock(32, 32);
	dim3 dimGrid((width - 1 + 32) / 32,
				 (height - 1 + 32) / 32);
    cuOverlayKernel<<<dimGrid, dimBlock, 0, ctx>>>(
            dst, step, width, height, xc, yc, r, value);

	return cudaSuccess;
}

int cuOverlayRect(
        unsigned char *dst, int step,
        int width, int height,
        int xc, int yc, int w, int h,
        unsigned char value,
        cudaStream_t& ctx) {
    dim3 dimBlock(32, 32);
    dim3 dimGrid((width - 1 + 32) / 32,
                 (height - 1 + 32) / 32);

    int x = xc - w/2;
    x = std::max(0, x);
    int xend = x + w;
    xend = std::min(xend, width);
    int y = yc - w/2;
    y = std::max(0, y);
    int yend = y + h;
    xend = std::min(yend, height);

    cuOverlayRectKernel<<<dimGrid, dimBlock, 0, ctx>>>(
            dst, step, width, height, x, y, x + w, y + h, value);

    return cudaSuccess;
}

