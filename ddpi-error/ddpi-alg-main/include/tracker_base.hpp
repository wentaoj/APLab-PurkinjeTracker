#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <string>
#include <atomic>

#include "ddpi_definition.hpp"
#include "gpu_array.hpp"
#include "gpu_2darray.hpp"
#include "gpu_pinned_array.hpp"

namespace ddpi {

class TrackerBase
{
public:
    enum {
        DDPI_OK     = 0x00,
        DDPI_BLINK  = 0x01,
        DDPI_NOTRACK = 0x02,
        DDPI_DISABLE = 0x10
    };
public:
	TrackerBase() {
        _initialized = false;
        _isTracking = false;
	};
	TrackerBase(TrackerBase&) = delete;
	
	virtual ~TrackerBase() {
		shutdown();
	};

	void enableTracking(bool track) {
        _isTracking = track;
	}

	virtual bool estimateP1() {
		return false;
	};

	virtual bool estimateP4() {
		return false;
	};

	void getCurrentData(DDPIdata_t& data);

    void getCurrentData(DDPIdata_t* data);

    void getCurrentData(std::shared_ptr<DDPIdata_t> data);

    virtual void visualization(float p4GUIscale=1.0f);

    void downloadDownSizedColorImage(unsigned char* img)
    { _dsColorImg.downloadAsync(img, _cudaStream); }

	void getDownSizedImageDimension(int &width, int &height) {
        width = _dsImg.width();
	    height = _dsImg.height();
    }

    void getImageDimension(int &width, int &height) {
        width = _imgWidth;
        height = _imgHeight;
    }

	int getGPUdev()
	{ return _gpuIdx; }

	bool isTracking()
	{ return _isTracking; }

	int height() { return _imgHeight; }

    void setROI(int x, int y, int width, int height);

    void setROI(const ROI_t &roi);

	virtual void setup(
		int imgWidth,
		int imgHeight,
		float downSamplingRate,
		int gpuDevice = 0);

	virtual void shutdown();

    virtual int track();

    int width() { return _imgWidth; }

	void upload(unsigned char* img, bool RDMA);

protected:
	void resizeRawImage();

	cudaStream_t _cudaStream;
	std::mutex _bufferMtx;
	float _downSamplingRate;
	float _x_scale;
	float _y_scale;
	Gpu2DArray<unsigned char> _dsImg;
    Gpu2DArray<unsigned char> _dsImgVis;
    GpuArray<unsigned char> _dsColorImg;
	int _gpuIdx;
    cudaDeviceProp _gpuProp;
	int _imgHeight;
	int _imgWidth;
    bool _initialized;
	bool _isTracking;
	NppStreamContext _nppStream;
	Gpu2DArray<unsigned char> _rawImg;

    Blob_t _p1;
    Blob_t _p4;
    ROI_t _roi;
    unsigned int _tag;
};

}; // source::ddpi