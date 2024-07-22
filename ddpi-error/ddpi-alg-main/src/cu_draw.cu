#include <driver_types.h>
#include "cu_helper.cuh"
#include "cu_draw.cuh"
#include <algorithm>

__global__ void cuDrawCircleKernel(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char R, unsigned char G, unsigned char B,
        float lw) {
    int tx = threadIdx.x + blockIdx.x * blockDim.x;
    int ty = threadIdx.y + blockIdx.y * blockDim.y;

    if ((tx > width) | (ty > height)) {
        return;
    }

    float dx = ((float)tx - xc);
    float dy = ((float)ty - yc);
    float d = sqrtf(dx * dx + dy * dy);
    if ((d >= r) && (d < (r + lw))) {
        int idx = 3 * (tx + ty * step);
        dst[idx] = R;
        dst[idx + 1] = G;
        dst[idx + 2] = B;
    }
}

__global__ void cuDrawRectKernel(
        unsigned char *dst, int step,
        int width, int height,
        int x, int y,
        int xend, int yend,
        unsigned char R, unsigned char G, unsigned char B,
        unsigned char lw) {
    int tx = threadIdx.x + blockIdx.x * blockDim.x;
    int ty = threadIdx.y + blockIdx.y * blockDim.y;

    if (((tx >= x) & (tx < xend) &
         (ty >= y) & (ty < yend)) &
        ((tx < x + lw) | (tx >= xend -lw) |
         (ty < y + lw) | (ty >= yend -lw))) {
        int idx = 3 * (tx + ty * step);
            dst[idx] = R;
            dst[idx + 1] = G;
            dst[idx + 2] = B;
    }
}

int cuDrawCircle(
        unsigned char *dst, int step,
        int width, int height,
        float xc, float yc, float r,
        unsigned char R, unsigned char G, unsigned char B,
        float lw,
		cudaStream_t& ctx) {
//    if ((xc < 0) | (xc >= width) | (yc < 0) | (yc >= width) | (r < 0)) {
//        return cudaSuccess;
//    }

	dim3 dimBlock(32, 32);
	dim3 dimGrid((width - 1 + 32) / 32,
				 (height - 1 + 32) / 32);
    cuDrawCircleKernel<<<dimGrid, dimBlock, 0, ctx>>>(
            dst, step, width, height, xc, yc, r, R, G, B, lw);

	return cudaSuccess;
}


int cuDrawRect(
        unsigned char *dst, int step,
        int width, int height,
        int xc, int yc, int w, int h,
        unsigned char R, unsigned char G, unsigned char B,
        unsigned char lw,
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

    cuDrawRectKernel<<<dimGrid, dimBlock, 0, ctx>>>(
            dst, step, width, height, x, y, x + w, y + h, R, G, B, lw);

    return cudaSuccess;
}
