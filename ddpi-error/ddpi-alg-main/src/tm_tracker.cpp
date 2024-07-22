#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "tm_tracker.hpp"
#include "cu_draw.cuh"
#include "cu_overlay.cuh"
#include "cu_gaussian.cuh"
#include "cu_image_moments.cuh"
#include "cu_radial_symmetry.cuh"
#include "cu_overlay.cuh"
#include "cu_utility.hpp"
#include "x_ddpi.hpp"

namespace ddpi
{

    bool TemplateMatchTracker::estimateP1()
    {
        // Apply image moments with a threshold to find P1
        unsigned char *srcPtr =
            _dsImg.gdata() + _dsP1ROI.x + _dsP1ROI.y * _dsImg.paddedWidth();
        cudaError_t cudaErrCode = imageMoments_8u32u(
            srcPtr, _dsImg.paddedWidth(),
            _gMoments_32i.gdata(),
            _dsP1ROI.width, _dsP1ROI.height,
            _p1Threshold,
            _dsImg.blocks(), _dsImg.threads(),
            _cudaStream);
        if (cudaErrCode != cudaSuccess)
        {
            THROW_ERROR_ARGS(
                XTracker,
                XTracker::ERR_P1_CENTER_OF_MASS,
                cudaGetErrorString(cudaErrCode));
        }
        _gMoments_32i.downloadAsync(_cudaStream);
        if (_gMoments_32i[0] == 0)
        {
            return false;
        }
        // Find center of mass
        int m10 = _gMoments_32i[1];
        int m01 = _gMoments_32i[2];

        float cx = (float)_gMoments_32i[1] / _gMoments_32i[0];
        float cy = (float)_gMoments_32i[2] / _gMoments_32i[0];

        // Find bottom left coner of P1 ROI
        int px = std::round(cx / _x_scale - _p1.bbox.width / 2.f + _p1ROI.x);
        px = std::max(0, px);
        px = std::min(px, _imgWidth - _p1.bbox.width);

        int py = std::round(cy / _y_scale - _p1.bbox.height / 2.f + _p1ROI.y);
        py = std::max(0, py);
        py = std::min(py, _imgHeight - _p1.bbox.height);

        _p1.bbox.x = px;
        _p1.bbox.y = py;
        return true;
    }

    bool TemplateMatchTracker::estimateP4()
    {
        NppStatus nppErr = NPP_SUCCESS;

        {
            // overlay
            overlayP1();

            unsigned char *srcPtr = _dsImg.gdata() + _dsP4ROI.x + _dsP4ROI.y * _dsImg.paddedWidth();

            // Calculate sqrare distance between template and image
            std::lock_guard<std::mutex> lk(_p4TemplateMtx);
            nppErr = nppiSqrDistanceSame_Norm_8u32f_C1R_Ctx(
                srcPtr, _dsImg.pitch(),
                {_dsP4ROI.width, _dsP4ROI.height},
                _p4Template.gdata(), _p4Template.pitch(),
                {_p4Template.width(), _p4Template.height()},
                _p4SqrDistanceBuf.gdata(), _p4SqrDistanceBuf.pitch(),
                _nppStream);
        }
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_NPP_SQRDISTANCE,
                             _cudaGetErrorEnum(nppErr));
        }

        // Find minimum index of square distance
        nppErr = nppiMinIndx_32f_C1R_Ctx(
            _p4SqrDistanceBuf.gdata(), _p4SqrDistanceBuf.pitch(),
            {_p4SqrDistanceBuf.width(), _p4SqrDistanceBuf.height()},
            _p4MinErrorBuf.gdata(),
            _gMinError.gdata(),
            _gMinIdx.gdata(), _gMinIdx.gdata() + 1,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_NPP_MIN_INDEX,
                             _cudaGetErrorEnum(nppErr));
        }
        _gMinError.downloadAsync(_cudaStream);
        _gMinIdx.downloadAsync(_cudaStream);

        _p4.score = _gMinError[0];

        // Find top left coner of P4 ROI
        int px = std::round(_gMinIdx[0] / _x_scale - _p4.bbox.width / 2.f + _p4ROI.x);
        px = std::max(0, px);
        px = std::min(px, _imgWidth - _p4.bbox.width);

        int py = std::round(_gMinIdx[1] / _y_scale - _p4.bbox.height / 2.f + _p4ROI.y);
        py = std::max(0, py);
        py = std::min(py, _imgHeight - _p4.bbox.height);

        _p4.bbox.x = px;
        _p4.bbox.y = py;
        return true;
    }

    void TemplateMatchTracker::getP1(float &x, float &y)
    {
        x = _p1.x;
        y = _p1.y;
    }

    void TemplateMatchTracker::getP1BoundingBox(int &x, int &y, int &width, int &height)
    {
        x = _p1.bbox.x;
        y = _p1.bbox.y;
        width = _p1.bbox.width;
        height = _p1.bbox.height;
    }

    void TemplateMatchTracker::getP1BoundingBox(ROI_t &roi)
    {
        roi = _p1.bbox;
    }

    void TemplateMatchTracker::getP1Image(float *img, int &width, int &height)
    {
        _p1Img.download(img);
        width = _p1Img.width();
        height = _p1Img.height();
    }

    void TemplateMatchTracker::getP1roi(int &x, int &y, int &width, int &height)
    {
        x = _p1ROI.x;
        y = _p1ROI.y;
        width = _p1ROI.width;
        height = _p1ROI.height;
    }

    void TemplateMatchTracker::getP1roi(ROI_t &roi)
    {
        roi.x = _p1ROI.x;
        roi.y = _p1ROI.y;
        roi.width = _p1ROI.width;
        roi.height = _p1ROI.height;
    }

    void TemplateMatchTracker::getP4(float &x, float &y)
    {
        x = _p4.x;
        y = _p4.y;
    }

    void TemplateMatchTracker::getP4BoundingBox(int &x, int &y, int &width, int &height)
    {
        x = _p4.bbox.x;
        y = _p4.bbox.y;
        width = _p4.bbox.width;
        height = _p4.bbox.height;
    }

    void TemplateMatchTracker::getP4BoundingBox(ROI_t &roi)
    {
        roi = _p4.bbox;
    }

    void TemplateMatchTracker::getP4Image(float *img)
    {
        _p4Img.download(img);
    }

    void TemplateMatchTracker::getP4roi(int &x, int &y, int &width, int &height)
    {
        x = _p4ROI.x;
        y = _p4ROI.y;
        width = _p4ROI.width;
        height = _p4ROI.height;
    }

    void TemplateMatchTracker::getP4roi(ROI_t &roi)
    {
        roi.x = _p4ROI.x;
        roi.y = _p4ROI.y;
        roi.width = _p4ROI.width;
        roi.height = _p4ROI.height;
    }

    void TemplateMatchTracker::getP4SqrDistanceImage(float *img)
    {
        _p4SqrDistanceBuf.download(img);
    }

    void TemplateMatchTracker::getP4Template(unsigned char *img, int &width, int &height)
    {
        _p4Template.download(img);
        width = _p4Template.width();
        height = _p4Template.height();
    }

    void TemplateMatchTracker::getROI(ROI_t &roi)
    {
        roi.x = _roi.x;
        roi.y = _roi.y;
        roi.width = _roi.width;
        roi.height = _roi.height;
    }

    bool TemplateMatchTracker::localizeP1(float threshold)
    {

        // Copy ROI and convert to float
        unsigned char *srcOffset =
            _rawImg.gdata() + _p1.bbox.x + _p1.bbox.y * _rawImg.paddedWidth();
        NppStatus nppErr = nppiScale_8u32f_C1R_Ctx(
            srcOffset, _rawImg.pitch(),
            _p1Img.gdata(), _p1Img.pitch(),
            {_p1Img.width(), _p1Img.height()},
            0.0f,
            255.0f,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P1_DOWNSIZE,
                             _cudaGetErrorEnum(nppErr));
        }

        //    // Gaussian Blur
        //    nppErr = nppiFilterGauss_32f_C1R_Ctx(
        //            _p1Img.gdata(), _p1Img.pitch(),
        //            _p1SmoothImg.gdata(), _p1SmoothImg.pitch(),
        //            {_p1SmoothImg.width(), _p1SmoothImg.height()},
        //            NPP_MASK_SIZE_3_X_3,
        //            _nppStream);
        //    if (nppErr != NPP_SUCCESS) {
        //        THROW_ERROR_ARGS(XTracker,
        //                XTracker::ERR_P1_GAUSSIAN_SMOOTH,
        //                _cudaGetErrorEnum(nppErr));
        //    }

        // Calcuate center of mass
        cudaError_t cudaErr = imageMoments_32f(
            _p1Img.gdata(), _p1Img.paddedWidth(),
            _gP1Moments_32f.gdata(),
            _p1Img.width(), _p1Img.height(),
            threshold,
            _p1Img.blocks(), _p1Img.threads(),
            _cudaStream);
        if (cudaErr != cudaSuccess)
        {
            // THROW_ERROR_ARGS(XTracker,
            //         XTracker::ERR_P1_LOCALIZE,
            // 	cudaGetErrorString(cudaErr));
        }

        _gP1Moments_32f.downloadAsync(_cudaStream);
        _p1.I = _gP1Moments_32f[0];
        if ((_p1.I == 0) | isnanf(_p1.I))
        {
            return false;
        }

        float cx = _gP1Moments_32f[1] / _p1.I;
        float cy = _gP1Moments_32f[2] / _p1.I;

        _p1.x = cx + _p1.bbox.x;
        _p1.y = cy + _p1.bbox.y;
        _p1.radius = sqrtf(_gP1Moments_32f[0] / threshold / M_PI);

        return true;
    }

    bool TemplateMatchTracker::localizeP4(float threshold)
    {
        cudaError_t cudaErr;

        // Copy ROI and convert to float
        unsigned char *srcOffset =
            _rawImg.gdata() + _p4.bbox.x + _p4.bbox.y * _rawImg.paddedWidth();
        NppStatus nppErr = nppiScale_8u32f_C1R_Ctx(
            srcOffset, _rawImg.pitch(),
            _p4Img.gdata(), _p4Img.pitch(),
            {_p4Img.width(), _p4Img.height()},
            0.0f,
            256.f,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P4_DOWNSIZE,
                             _cudaGetErrorEnum(nppErr));
        }

        // Calcuate center of mass to refine ROI
        cudaErr = imagePowerMoments_32f(
            _p4Img.gdata(), _p4Img.paddedWidth(),
            _gP4Moments_32f.gdata(),
            _p4Img.width(), _p4Img.height(),
            4,
            _p4Img.blocks(), _p4Img.threads(),
            _cudaStream);
        _gP4Moments_32f.downloadAsync(_cudaStream);

        float I4 = _gP4Moments_32f[0];
        if (I4 > 0)
        {
            // refine ROI
            float qx = _gP4Moments_32f[1] / I4;
            float qy = _gP4Moments_32f[2] / I4;
            int px = std::round(qx + _p4.bbox.x - _p4.bbox.width / 2.f);
            px = std::max(0, px);
            px = std::min(px, _imgWidth - _p4.bbox.width);

            int py = std::round(qy + _p4.bbox.y - _p4.bbox.height / 2.f);
            py = std::max(0, py);
            py = std::min(py, _imgHeight - _p4.bbox.height);

            _p4.bbox.x = px;
            _p4.bbox.y = py;

            srcOffset =
                _rawImg.gdata() + _p4.bbox.x + _p4.bbox.y * _rawImg.paddedWidth();
            nppErr = nppiScale_8u32f_C1R_Ctx(
                srcOffset, _rawImg.pitch(),
                _p4Img.gdata(), _p4Img.pitch(),
                {_p4Img.width(), _p4Img.height()},
                0.0f,
                256.f,
                _nppStream);
        }

        // Gaussian Blur
        nppErr = nppiFilterGauss_32f_C1R_Ctx(
            _p4Img.gdata(), _p4Img.pitch(),
            _p4SmoothImg.gdata(), _p4SmoothImg.pitch(),
            {_p4Img.width(), _p4Img.height()},
            NPP_MASK_SIZE_3_X_3,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P4_GAUSSIAN_SMOOTH,
                             _cudaGetErrorEnum(nppErr));
        }

        // Calcuate center of mass
        cudaErr = imageMoments_32f(
            _p4SmoothImg.gdata(), _p4SmoothImg.paddedWidth(),
            _gP4Moments_32f.gdata(),
            _p4SmoothImg.width(), _p4SmoothImg.height(),
            threshold,
            _p4SmoothImg.blocks(), _p4SmoothImg.threads(),
            _cudaStream);
        if (cudaErr != cudaSuccess)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P4_LOCALIZE,
                             cudaGetErrorString(cudaErr));
        }

        _gP4Moments_32f.downloadAsync(_cudaStream);
        _p4.I = _gP4Moments_32f[0];
        if (_p4.I == 0)
        {
            return false;
        }

        float cx = _gP4Moments_32f[1] / _p4.I;
        float cy = _gP4Moments_32f[2] / _p4.I;

        // Calculate gradient image
        nppErr = nppiGradientVectorSobelBorder_32f_C1R_Ctx(
            _p4SmoothImg.gdata(), _p4SmoothImg.pitch(),
            {_p4SmoothImg.width(), _p4SmoothImg.height()},
            {0, 0},
            _p4DX.gdata(), _p4DX.pitch(),
            _p4DY.gdata(), _p4DY.pitch(),
            _p4DI.gdata(), _p4DI.pitch(),
            _p4Angle.gdata(), _p4Angle.pitch(),
            {_p4SmoothImg.width(), _p4SmoothImg.height()},
            NPP_MASK_SIZE_3_X_3,
            nppiNormL2,
            NPP_BORDER_REPLICATE,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P4_LOCALIZE,
                             _cudaGetErrorEnum(nppErr));
        }

        // Radial Symmetry
        cudaErr = radialSymmetry_32f(
            _p4DX.gdata(),
            _p4DY.gdata(),
            _p4DI.gdata(),
            _p4Angle.gdata(),
            _p4Img.gdata(),
            _p4DX.paddedWidth(),
            _p4Img.width(), _p4Img.height(),
            cx, cy,
            0.0f,
            _gRadialSymmetry_32f.gdata(),
            _p4SmoothImg.blocks(), _p4SmoothImg.threads(),
            _cudaStream);
        if (cudaErr != cudaSuccess)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_P4_LOCALIZE,
                             cudaGetErrorString(cudaErr));
        }

        _gRadialSymmetry_32f.downloadAsync(_cudaStream);
        float det = _gRadialSymmetry_32f[2] * _gRadialSymmetry_32f[2] -
                    _gRadialSymmetry_32f[4] * _gRadialSymmetry_32f[0];
        if (det == 0)
        {
            return false;
        }

        float x = (_gRadialSymmetry_32f[3] * _gRadialSymmetry_32f[0] -
                   _gRadialSymmetry_32f[2] * _gRadialSymmetry_32f[1]) /
                  det;
        float y = (_gRadialSymmetry_32f[3] * _gRadialSymmetry_32f[2] -
                   _gRadialSymmetry_32f[4] * _gRadialSymmetry_32f[1]) /
                  det;
        float r = std::sqrt(_gRadialSymmetry_32f[5] / _gP4Moments_32f[0]);

        _p4.x = x + _p4.bbox.x;
        _p4.y = y + _p4.bbox.y;
        _p4.radius = r;

        _p4mx += _p4.x;
        _p4my += _p4.y;
        _p4mIdx += 1;
        //_p4.I = qx + _p4.bbox.x;
        //_p4.score = qy + _p4.bbox.y;
        return true;
    }

    void TemplateMatchTracker::setP1BoundingBoxSize(int bboxSize)
    {

        if ((bboxSize <= 0) | (bboxSize > _imgWidth) | (bboxSize > _imgHeight))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "The ROI size of first Purkinje image is invalid");
        }
        _p1.bbox.width = bboxSize;
        _p1.bbox.height = bboxSize;

        _p1Img.create(_p1.bbox.width, _p1.bbox.height);
        if (!getNumBlocksAndThreadsForReduction(_gpuProp, _p1Img))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "data input is too large, please choose a smaller number");
        }

        _p1SmoothImg.create(_p1.bbox.width, _p1.bbox.height);
        _p1DX.create(_p1.bbox.width, _p1.bbox.height);
        _p1DY.create(_p1.bbox.width, _p1.bbox.height);
        _p1DI.create(_p1.bbox.width, _p1.bbox.height);
        _p1Angle.create(_p1.bbox.width, _p1.bbox.height);
    }

    void TemplateMatchTracker::setP1Threshold(int p1Threshold)
    {
        _p1Threshold = p1Threshold;
    }

    void TemplateMatchTracker::setP1roi(int x, int y, int width, int height)
    {
        _dsP1ROI.x = std::round(x * _x_scale);
        _dsP1ROI.y = std::round(y * _y_scale);
        _dsP1ROI.width = std::round(width * _x_scale);
        _dsP1ROI.height = std::round(height * _y_scale);

        _p1ROI.x = std::round(_dsP1ROI.x / _x_scale);
        _p1ROI.y = std::round(_dsP1ROI.y / _y_scale);
        _p1ROI.width = std::round(_dsP1ROI.width / _x_scale);
        _p1ROI.height = std::round(_dsP1ROI.height / _y_scale);
    }

    void TemplateMatchTracker::setP1roi(const ROI_t &roi)
    {
        setP1roi(roi.x, roi.y, roi.width, roi.height);
    }

    void TemplateMatchTracker::setP4BoundingBoxSize(int bboxSize)
    {

        if ((bboxSize <= 0) | (bboxSize > _imgWidth) | (bboxSize > _imgHeight))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "The ROI size of fourth Purkinje image is invalid");
        }
        _p4.bbox.width = bboxSize;
        _p4.bbox.height = bboxSize;

        _p4Img.create(_p4.bbox.width, _p4.bbox.height);
        if (!getNumBlocksAndThreadsForReduction(_gpuProp, _p4Img))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "data input is too large, please choose a smaller number");
        }

        _p4SmoothImg.create(_p4.bbox.width, _p4.bbox.height);
        if (!getNumBlocksAndThreadsForReduction(_gpuProp, _p4SmoothImg))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "data input is too large, please choose a smaller number");
        }

        _p4DX.create(_p4.bbox.width, _p4.bbox.height);
        _p4DY.create(_p4.bbox.width, _p4.bbox.height);
        _p4DI.create(_p4.bbox.width, _p4.bbox.height);
        _p4Angle.create(_p4.bbox.width, _p4.bbox.height);

        templateSize_ = std::round(bboxSize * _x_scale);
        _p4Template.create(templateSize_, templateSize_);
    }

    void TemplateMatchTracker::setP4roi(int x, int y, int width, int height)
    {
        _dsP4ROI.x = std::round(x * _x_scale);
        _dsP4ROI.y = std::round(y * _y_scale);
        _dsP4ROI.width = std::round(width * _x_scale);
        _dsP4ROI.height = std::round(height * _y_scale);

        _p4ROI.x = std::round(_dsP4ROI.x / _x_scale);
        _p4ROI.y = std::round(_dsP4ROI.y / _y_scale);
        _p4ROI.width = std::round(_dsP4ROI.width / _x_scale);
        _p4ROI.height = std::round(_dsP4ROI.height / _y_scale);

        // allocate template match buffer
        _p4SqrDistanceBuf.release();
        _p4SqrDistanceBuf.create(_dsP4ROI.width, _dsP4ROI.height);

        // allocate find minimum index buffer
        int bufSize;
        NppStatus nppErr = nppiMinIndxGetBufferHostSize_8u_C1R_Ctx(
            {_p4SqrDistanceBuf.width(), _p4SqrDistanceBuf.height()},
            &bufSize,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             _cudaGetErrorEnum(nppErr));
        }
        _p4MinErrorBuf.release();
        _p4MinErrorBuf.create(bufSize);
    }

    void TemplateMatchTracker::setP4roi(const ROI_t &roi)
    {
        setP4roi(roi.x, roi.y, roi.width, roi.height);
    }

    void TemplateMatchTracker::setP4Template(unsigned char amp, float sigma)
    {
        if ((amp <= 0) | (sigma <= 0))
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "The parameters of gaussian template are invalid");
        }

        if (_p4Template.empty())
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             "The parameters of gaussian template are invalid");
        }

        {
            std::lock_guard<std::mutex> lk(_p4TemplateMtx);
            cuGaussain_8u(
                _p4Template.gdata(), _p4Template.paddedWidth(),
                _p4Template.width(), _p4Template.height(),
                amp, sigma,
                _cudaStream);
        }
    }

    void TemplateMatchTracker::setROI(int x, int y, int width, int height)
    {
        setP1roi(x, y, width, height);
        setP4roi(x, y, width, height);
        _roi.x = _p1ROI.x;
        _roi.y = _p1ROI.y;
        _roi.width = _p1ROI.width;
        _roi.height = _p1ROI.height;
    }

    void TemplateMatchTracker::setROI(ROI_t &roi)
    {
        setP1roi(roi.x, roi.y, roi.width, roi.height);
        setP4roi(roi.x, roi.y, roi.width, roi.height);
        _roi.x = _p1ROI.x;
        _roi.y = _p1ROI.y;
        _roi.width = _p1ROI.width;
        _roi.height = _p1ROI.height;
    }

    void TemplateMatchTracker::setup(
        int imgWidth,
        int imgHeight,
        float downSamplingRate,
        int gpuDevice)
    {
        if (_initialized)
            return;

        // Setup tracker_base
        TrackerBase::setup(
            imgWidth,
            imgHeight,
            downSamplingRate,
            gpuDevice);

        // Set ROI as whole image
        _dsP1ROI.x = 0;
        _dsP1ROI.y = 0;
        _dsP1ROI.width = _dsImg.width();
        _dsP1ROI.height = _dsImg.height();

        _dsP4ROI.x = 0;
        _dsP4ROI.y = 0;
        _dsP4ROI.width = _dsImg.width();
        _dsP4ROI.height = _dsImg.height();

        _p1ROI.x = 0;
        _p1ROI.y = 0;
        _p1ROI.width = _dsImg.width();
        _p1ROI.height = _dsImg.height();

        _p4ROI.x = 0;
        _p4ROI.y = 0;
        _p4ROI.width = _dsImg.width();
        _p4ROI.height = _dsImg.height();

        // allocate pinned memory for image moments
        _gMoments_32i.create(3);
        _gP1Moments_32f.create(3);
        _gP4Moments_32f.create(3);
        _gRadialSymmetry_32f.create(6);
        _gMinIdx.create(2);
        _gMinError.create(1);

        // allocate template match buffer
        _p4SqrDistanceBuf.create(_dsImg.width(), _dsImg.height());

        // allocate find minimum index buffer
        int bufSize;
        NppStatus nppErr = nppiMinIndxGetBufferHostSize_8u_C1R_Ctx(
            {_p4SqrDistanceBuf.width(), _p4SqrDistanceBuf.height()},
            &bufSize,
            _nppStream);
        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             _cudaGetErrorEnum(nppErr));
        }
        _p4MinErrorBuf.create(bufSize);

        _initialized = true;

        _p4mx = 0;
        _p4my = 0;
        _p4mIdx = 0;
        return;
    }

    void TemplateMatchTracker::setup(
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
        bool tracking)
    {

        setup(imgWidth, imgHeight, downSamplingRate, gpuDevice);
        setNoise(noise);
        setP1Threshold(p1Threshold);
        setP1BoundingBoxSize(p1ROIsize);
        setP4BoundingBoxSize(p4ROIsize);
        setP4Template(p4Intensity, p4Radius);
        setOverlayScale(overlayScale);
        setP4GUIScale(p4GUIScale);
        enableTracking(tracking);
    }

    void TemplateMatchTracker::shutdown()
    {
        if (!_initialized)
            return;

        TrackerBase::shutdown();

        // Release image related to P1
        _p1Img.release();
        _p1SmoothImg.release();
        _p1DX.release();
        _p1DY.release();
        _p1DI.release();
        _p1Angle.release();

        // Release image related to P4
        _p4Img.release();
        _p4SmoothImg.release();
        _p4DX.release();
        _p4DY.release();
        _p4DI.release();
        _p4Angle.release();
        _p4Template.release();

        // Release buffer
        _gMoments_32i.release();
        _gP1Moments_32f.release();
        _gP4Moments_32f.release();
        _gRadialSymmetry_32f.release();
        _gMinIdx.release();
        _gMinError.release();

        _p4SqrDistanceBuf.release();
        _p4MinErrorBuf.release();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int TemplateMatchTracker::track()
    {
        if (!_isTracking)
            return DDPI_DISABLE;

        _tag = DDPI_OK;

        // Estimate P1
        if (estimateP1())
        {
            // Localize P1 by radial symmetry
            if (!localizeP1(128))
            {
                _tag |= DDPI_BLINK;
            }
            else
            {
                //            overlayP1();
            }
        }
        else
        {
            _tag |= DDPI_BLINK;
        }

        // Estimate P4
        estimateP4();

        // Localize P4 by radial symmetry
        if (!localizeP4(_noise))
        {
            _tag |= DDPI_NOTRACK;
        }

        // cudaDeviceSynchronize();
        return _tag;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void TemplateMatchTracker::visualization(float p4GUIscale)
    {
        TrackerBase::visualization();

        auto nppErr = nppiDup_8u_C1C3R_Ctx(
            _dsImgVis.gdata(), _dsImgVis.pitch(),
            _dsColorImg.gdata(), _dsImgVis.width() * 3,
            {_dsImgVis.width(), _dsImgVis.height()},
            _nppStream);

        if (nppErr != NPP_SUCCESS)
        {
            THROW_ERROR_ARGS(XTracker,
                             XTracker::ERR_TRACKER_SETUP_FAILURE,
                             _cudaGetErrorEnum(nppErr));
        }

        if ((_tag & DDPI_BLINK) == 0)
        {
            cuDrawCircle(
                _dsColorImg.gdata(), _dsImg.width(),
                _dsImgVis.width(), _dsImgVis.height(),
                _p1.x * _x_scale, _p1.y * _y_scale, _p1.radius * _x_scale,
                255, 0, 0,
                1.25f,
                _cudaStream);

            cuDrawRect(
                _dsColorImg.gdata(), _dsImg.width(),
                _dsImgVis.width(), _dsImgVis.height(),
                static_cast<int>(std::round(_p1.x * _x_scale)),
                static_cast<int>(std::round(_p1.y * _y_scale)),
                static_cast<int>(std::round(_p1.bbox.width * _x_scale * _overlayScale)),
                static_cast<int>(std::round(_p1.bbox.height * _y_scale * _overlayScale)),
                0, 255, 0,
                1,
                _cudaStream);
        }

        if ((_tag & DDPI_NOTRACK) == 0)
        {
            cuDrawCircle(
                _dsColorImg.gdata(), _dsImg.width(),
                _dsImgVis.width(), _dsImgVis.height(),
                _p4.x * _x_scale, _p4.y * _y_scale, _p4.radius * _x_scale * _P4GUIScale,
                255, 0, 255,
                1.25f,
                _cudaStream);

            cuDrawCircle(
                _dsColorImg.gdata(), _dsImg.width(),
                _dsImgVis.width(), _dsImgVis.height(),
                _p4mx / _p4mIdx * _x_scale, _p4my / _p4mIdx * _y_scale, 4 * _p4.radius * _x_scale * _P4GUIScale,
                255, 255, 0,
                1.25f,
                _cudaStream);
            _p4mx = 0;
            _p4my = 0;
            _p4mIdx = 0;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void TemplateMatchTracker::overlayP1()
    {
        //    cuOverlay(
        //            _dsImg.gdata(), _dsImg.paddedWidth(),
        //            _dsImg.width(), _dsImg.height(),
        //            _p1.x * _x_scale, _p1.y * _y_scale, _p1.radius * _x_scale,
        //            0,
        //            _cudaStream);
        cuOverlayRect(
            _dsImg.gdata(), _dsImg.paddedWidth(),
            _dsImg.width(), _dsImg.height(),
            static_cast<int>(std::round(_p1.x * _x_scale)),
            static_cast<int>(std::round(_p1.y * _y_scale)),
            static_cast<int>(std::round(_p1.bbox.width * _x_scale * _overlayScale)),
            static_cast<int>(std::round(_p1.bbox.height * _y_scale * _overlayScale)),
            0,
            _cudaStream);
    }
}; // source::ddpi