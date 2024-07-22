#pragma once

void cuGaussain_8u(
		unsigned char *pSrc, int step,
		int width, int height,
		unsigned char amp,
		float sigma,
		cudaStream_t& ctx);

void cuGaussain(
	float *pSrc, int step,
	int width, int height,
	float amp,
	float sigma,
	cudaStream_t& ctx);
