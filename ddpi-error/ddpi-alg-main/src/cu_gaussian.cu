#include "cu_helper.cuh"
#include "cu_gaussian.cuh"

__global__ void cuGaussianKernel_8u(
		unsigned char *pSrc, int step,
		int width, int height,
		unsigned char amp,
		float sigma) {
	int tx = threadIdx.x + blockIdx.x * blockDim.x;
	int ty = threadIdx.y + blockIdx.y * blockDim.y;
	if (tx < width && ty < height)
	{
		float x = tx - (float) width / 2.0f;
		float y = ty - (float) height / 2.0f;
		float r2 = x * x + y * y;
		float val = amp * exp(-r2 / 2.0f / sigma / sigma);
		pSrc[tx + ty * step] = round(val);
	}
}

__global__ void cuGaussianKernel(
		float *pSrc, int step,
		int width, int height,
		float amp,
		float sigma) {
	int tx = threadIdx.x + blockIdx.x * blockDim.x;
	int ty = threadIdx.y + blockIdx.y * blockDim.y;
	if (tx < width && ty < height)
	{
		float x = tx - (float) width / 2.0f;
		float y = ty - (float) height / 2.0f;
		float r2 = x * x + y * y;
		pSrc[tx + ty * step] = amp * exp(-r2 / 2.0f / sigma / sigma);
	}
}

void cuGaussain_8u(
		unsigned char *pSrc, int step,
		int width, int height,
		unsigned char amp,
		float sigma,
		cudaStream_t& ctx) {

	dim3 dimBlock(32, 32);
	dim3 dimGrid((width - 1 + 32) / 32,
				 (height - 1 + 32) / 32);
	cuGaussianKernel_8u<<<dimGrid, dimBlock, 0, ctx>>>(pSrc, step, width, height, amp, sigma);

	return;
}

void cuGaussain(
		float *pSrc, int step,
		int width, int height,
		float amp,
		float sigma,
		cudaStream_t& ctx) {

	dim3 dimBlock(32, 32);
	dim3 dimGrid((width - 1 + 32) / 32,
		         (height - 1 + 32) / 32);
	cuGaussianKernel<<<dimGrid, dimBlock, 0, ctx>>>(pSrc, step, width, height, amp, sigma);
	
	return;
}