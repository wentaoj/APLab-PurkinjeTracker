#include <cmath>
#include <memory>

#include "tracker_base.hpp"
#include "cu_utility.hpp"
#include "cu_draw.cuh"
#include "x_ddpi.hpp"

namespace ddpi {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::visualization(float p4GUIscale) {
    NppStatus nppErr = nppiDup_8u_C1C3R_Ctx(
            _dsImgVis.gdata(), _dsImgVis.pitch(),
            _dsColorImg.gdata(), 3 * _dsImgVis.width() * sizeof(unsigned char),
            {_dsImgVis.width(), _dsImgVis.height()},
            _nppStream);
    if (nppErr != NPP_SUCCESS) {
        // THROW_ERROR_ARGS(XTracker,
        //         XTracker::ERR_NPP_COLOR_CONVERSION,
        //         _cudaGetErrorEnum(nppErr));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::getCurrentData(DDPIdata_t& data) {
    data.p1 = _p1;
    data.p4 = _p4;
    data.tag = _tag;
};

void TrackerBase::getCurrentData(DDPIdata_t *data) {
    data->p1 = _p1;
    data->p4 = _p4;
    data->tag = _tag;
};

void TrackerBase::getCurrentData(std::shared_ptr<DDPIdata_t> data) {
    data->p1 = _p1;
    data->p4 = _p4;
    data->tag = _tag;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::resizeRawImage() {
	//_status = DDPI_IMAGE_RESIZE_FAILURE;

	// Down sampling raw image
	if (_downSamplingRate < 1) {
        NppStatus nppErr = nppiResize_8u_C1R_Ctx(
                _rawImg.gdata(), _rawImg.pitch(),
                {_rawImg.width(), _rawImg.height()},
                {0, 0, _rawImg.width(), _rawImg.height()},
                _dsImg.gdata(), _dsImg.pitch(),
                {_dsImg.width(), _dsImg.height()},
                {0, 0, _dsImg.width(), _dsImg.height()},
                NPPI_INTER_LINEAR,
                _nppStream);
        if (nppErr != NPP_SUCCESS) {
            THROW_ERROR_ARGS(XTracker,
                    XTracker::ERR_NPP_RESIZE,
                    _cudaGetErrorEnum(nppErr));
        }

        nppErr = nppiCopy_8u_C1R(
                _dsImg.gdata(), _dsImg.pitch(),
                _dsImgVis.gdata(), _dsImgVis.pitch(),
                {_dsImg.width(), _dsImg.height()});
        if (nppErr != NPP_SUCCESS) {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_NPP_RESIZE,
                             _cudaGetErrorEnum(nppErr));
        }

        // Sync GPU
//        cudaError_t cudaErr = cudaDeviceSynchronize();
//        if (cudaErr != cudaSuccess) {
//            THROW_ERROR_ARGS(XTracker,
//                    XTracker::ERR_NPP_RESIZE,
//                    _cudaGetErrorEnum(nppErr));
//        }
	} else {
	    _rawImg.copyTo(_dsImg);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::setROI(const ROI_t &roi) {
    _roi = roi;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::setup(
        int imgWidth,
        int imgHeight,
        float downSamplingRate,
        int gpuDevice) {
    if (_initialized) return;

	// Check argument
	int size = imgWidth * imgHeight;
	if (size <= 0) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_TRACKER_SETUP_FAILURE,
                "Image dimension is invalid");
	}

	if ((downSamplingRate <= 0) | (downSamplingRate > 1)) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_TRACKER_SETUP_FAILURE,
                "Down-sampling Rate is invalid");
	}

	// Set variable
	_imgWidth = imgWidth;
	_imgHeight = imgHeight;
	_downSamplingRate = downSamplingRate;

	// Find gpu 
	int nDevices;
    cudaError code = cudaGetDeviceCount(&nDevices);
	if (code != cudaSuccess) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_NOT_SUPPORTED,
				cudaGetErrorString(code));
	}

	if (nDevices == 0) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_NOT_FOUND,
				"Could not find Nvidia GPU");
	}

	// Set gpu device
	if (gpuDevice > nDevices - 1) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_INVALID_DEVICE,
				"Invalid GPU device");
	}
	_gpuIdx = gpuDevice;

	code = cudaSetDevice(gpuDevice);
	if (code != cudaSuccess) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_INVALID_DEVICE,
				cudaGetErrorString(code));
	}

    code = cudaGetDeviceProperties(&_gpuProp, gpuDevice);
    if (code != cudaSuccess) {
        THROW_ERROR_ARGS(XTracker,
                         XTracker::ERR_GPU_INVALID_DEVICE,
                         cudaGetErrorString(code));
    }

	// Set cuda & npp stream
	code = cudaStreamCreate(&_cudaStream);
	if (code != cudaSuccess) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_STREAM_FAILURE,
				cudaGetErrorString(code));
	}
	NppStatus nppErr = nppSetStream(_cudaStream);
	if (nppErr != NPP_SUCCESS) {
        THROW_ERROR_ARGS(
                XTracker,
                XTracker::ERR_GPU_STREAM_FAILURE,
				_cudaGetErrorEnum(code));
	}
	nppErr = nppGetStreamContext(&_nppStream);
	if (nppErr != NPP_SUCCESS) {
        THROW_ERROR_ARGS(XTracker,
                XTracker::ERR_GPU_STREAM_FAILURE,
				_cudaGetErrorEnum(code));
	}

	// Allocate raw image
	_rawImg.create(_imgWidth, _imgHeight);

	// Allocate down sampling image
	int dsImgWidth = static_cast<int>(std::round(_imgWidth * _downSamplingRate));
	int dsImgHeight = static_cast<int>(std::round(_imgHeight * _downSamplingRate));
    _x_scale = static_cast<float>(dsImgWidth) / _imgWidth;
    _y_scale = static_cast<float>(dsImgHeight) / _imgHeight;
	_dsImg.create(dsImgWidth, dsImgHeight);
    if (!getNumBlocksAndThreadsForReduction(_gpuProp, _dsImg)) {
        THROW_ERROR_ARGS(XTracker,
                         XTracker::ERR_TRACKER_SETUP_FAILURE,
                         "data input is too large, please choose a smaller number");
    }

    _dsImgVis.create(dsImgWidth, dsImgHeight);
    _dsColorImg.create(dsImgWidth * dsImgHeight * 3);
    _initialized = true;
	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::shutdown() {
    if (!_initialized) return;

	// Releae raw image
	_rawImg.release();
    _dsImg.release();
    _dsImgVis.release();
    _dsColorImg.release();

    if (_cudaStream) {
        cudaStreamDestroy(_cudaStream);
        _cudaStream = nullptr;
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TrackerBase::track() {
    if (!(_isTracking && _initialized)) return DDPI_DISABLE;
    return DDPI_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TrackerBase::upload(unsigned char* img, bool RDMA) {
    if (!_initialized) return;

	// upload to raw image
	_rawImg.uploadAsync(img, _cudaStream, RDMA);

	// down sampling image
    resizeRawImage();

//    cudaStreamSynchronize(_cudaStream);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}; // source::ddpi