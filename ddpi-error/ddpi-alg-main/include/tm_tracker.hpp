#pragma once

#include "gpu_pinned_array.hpp"
#include "tracker_base.hpp"

namespace ddpi
{

	class TemplateMatchTracker : public TrackerBase
	{
	public:
		TemplateMatchTracker() : TrackerBase(){};
		TemplateMatchTracker(TemplateMatchTracker &) = delete;

		~TemplateMatchTracker(){};

		bool estimateP1() final;

		bool estimateP4() final;

		void getP1(float &x, float &y);

		void getP1BoundingBox(int &x, int &y, int &width, int &height);

		void getP1BoundingBox(ROI_t &roi);

		void getP1Image(float *img, int &width, int &height);

		void getP1roi(int &x, int &y, int &width, int &height);

		void getP1roi(ROI_t &roi);

		void getP4(float &x, float &y);

		void getP4BoundingBox(int &x, int &y, int &width, int &height);

		void getP4BoundingBox(ROI_t &roi);

		void getP4Image(float *img);

		void getP4roi(int &x, int &y, int &width, int &height);

		void getP4roi(ROI_t &roi);

		void getP4Template(unsigned char *img, int &width, int &height);

		void getP4SqrDistanceImage(float *img);

		void getROI(ROI_t &roi);

		bool localizeP1(float threadshold);

		bool localizeP4(float threadshold);

		void overlayP1();

		void setP1BoundingBoxSize(int bboxSize);

		void setP1roi(int x, int y, int width, int height);

		void setP1roi(const ROI_t &roi);

		void setP1Threshold(int p1Threshold);

		// void setP1TotalIntensityLimit(int min, int max);

		void setP4BoundingBoxSize(int bboxSize);

		void setP4roi(int x, int y, int width, int height);

		void setP4roi(const ROI_t &roi);

		void setP4Template(unsigned char amp, float sigma);

		void setROI(int x, int y, int width, int height);

		void setROI(ROI_t &roi);

		void setNoise(int noise)
		{
			_noise = noise;
		}

		void setOverlayScale(float overlayScale)
		{
			_overlayScale = overlayScale;
		}

		void setP4GUIScale(float p4GUIScale)
		{
			_P4GUIScale = p4GUIScale;
		}

		void setup(
			int imgWidth,
			int imgHeight,
			float downSamplingRate,
			int gpuDevice = 0) final;

		void setup(
			int imgWidth,
			int imgHeight,
			float downSamplingRate,
			int gpuDevice,
			int noise,
			float overlayScale,
			int p1ROIsize,
			int p1Threshold,
			int p4ROIsize,
			int p4Intensity,
			float p4Radius,
			float p4GUIScale,
			bool tracking);

		void shutdown() final;

		int track() final;

		void getImage(unsigned char *img)
		{
			_dsImg.download(img);
		}

		void getP4TemplateSize(int *size)
		{
			*size = templateSize_;
		}

		virtual void visualization(float p4GUIscale = 1.0f) final;

		void setDevice()
		{
			cudaSetDevice(_gpuIdx);
		}

	private:
		Gpu2DArray<float> _p1Angle;
		Gpu2DArray<float> _p1DX;
		Gpu2DArray<float> _p1DY;
		Gpu2DArray<float> _p1DI;
		Gpu2DArray<float> _p1Img;
		Gpu2DArray<float> _p1SmoothImg;

		Gpu2DArray<float> _p4Angle;
		Gpu2DArray<float> _p4DX;
		Gpu2DArray<float> _p4DY;
		Gpu2DArray<float> _p4DI;
		Gpu2DArray<float> _p4Img;
		GpuArray<unsigned char> _p4MinErrorBuf;
		Gpu2DArray<float> _p4SmoothImg;
		Gpu2DArray<float> _p4SqrDistanceBuf;
		Gpu2DArray<unsigned char> _p4Template;

		ROI_t _dsP1ROI;
		ROI_t _dsP4ROI;
		int _noise;
		float _overlayScale;
		int _p1Threshold;
		ROI_t _p1ROI;
		ROI_t _p4ROI;
		ROI_t _roi;
		float _P4GUIScale;

		GpuPinnedArray<float> _gMinError;
		GpuPinnedArray<int> _gMinIdx;
		GpuPinnedArray<unsigned int> _gMoments_32i;
		GpuPinnedArray<float> _gP1Moments_32f;
		GpuPinnedArray<float> _gP4Moments_32f;
		GpuPinnedArray<float> _gRadialSymmetry_32f;

		std::mutex _p4TemplateMtx;

		float _p4mx;
		float _p4my;
		float _p4mIdx;

		int templateSize_;
	};

}; // source::ddpi