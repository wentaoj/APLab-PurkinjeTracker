//
// Copyright (c) 2017 Santini Designs. All rights reserved.
//

#pragma once

#include <string>

#include "exception.hpp"

class [[maybe_unused]] XGpuObject : public XException<XGpuObject>
{
public:
    enum {
        ERR_GPUOBJECT_OPERATION_INVALID_DIMENSION,
        ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
        ERR_GPUOBJECT_NOT_ALLOCATED,
        ERR_GPUOBJECT_CUDA_COPY,
        ERR_GPUOBJECT_CUDA_FREE,
        ERR_GPUOBJECT_CUDA_MALLOC,
        ERR_GPUOBJECT_CUDA_HOST_FREE,
        ERR_GPUOBJECT_CUDA_HOST_MALLOC,
    };

    /**
     * @brief Default constructor.
     *
     * @param[in] gravity Gravity of the exception
     * @param[in] filename File name in which the exception has occurred
     * @param[in] line Line number
     * @param[in] function Function in which the exception has occurred
     * @param[in] code Error code for this exception
     * @param[in] arguments Additional arguments for the exception
     */
    template<typename... ARGUMENTS>
            XGpuObject(const severity_e severity,
                       const std::string& fileName,
                       unsigned int line,
                       const std::string& function,
                       const error_code_t code,
                       const ARGUMENTS... arguments) :
                XException<XGpuObject>(severity, fileName, line, function, code, arguments...)
    {
        // Nothing to do
    }

    /// @see XException::decoder()
    [[maybe_unused]]
    static std::string decoder(error_code_t code)
    {
        switch (code)
        {
            case ERR_GPUOBJECT_OPERATION_INVALID_DIMENSION:
            case ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT:
            case ERR_GPUOBJECT_NOT_ALLOCATED:
                return "GpuObject Error: {}";
            case ERR_GPUOBJECT_CUDA_COPY:
                return "GpuObject Error: copy operation gave {}";
            case ERR_GPUOBJECT_CUDA_FREE:
                return "GpuObject Error: free operation gave {}";
            case ERR_GPUOBJECT_CUDA_MALLOC:
                return "GpuObject Error: malloc operation gave {}";
            case ERR_GPUOBJECT_CUDA_HOST_FREE:
                return "GpuObject Error: host free operation gave {}";
            case ERR_GPUOBJECT_CUDA_HOST_MALLOC:
                return "GpuObject Error: host malloc operation gave {}";
            default:
                return "GpuObject Error: unknown error";
        }
    }
}; // XGpuObject


class [[maybe_unused]] XTracker : public XException<XTracker>
{
public:
    enum {
        ERR_GPU_INVALID_DEVICE,
        ERR_GPU_NOT_FOUND,
        ERR_GPU_NOT_SUPPORTED,
        ERR_GPU_STREAM_FAILURE,
        ERR_NPP_COLOR_CONVERSION,
        ERR_NPP_RESIZE,
        ERR_NPP_SQRDISTANCE,
        ERR_NPP_MIN_INDEX,
        ERR_P1_CENTER_OF_MASS,
        ERR_P1_LOCALIZE,
        ERR_P1_DOWNSIZE,
        ERR_P1_GAUSSIAN_SMOOTH,
        ERR_P4_CENTER_OF_MASS,
        ERR_P4_ROI_FAILURE,
        ERR_P4_DOWNSIZE,
        ERR_P4_LOCALIZE,
        ERR_P4_GAUSSIAN_SMOOTH,
        ERR_TRACKER_SETUP_FAILURE,
    };

    /**
     * @brief Default constructor.
     *
     * @param[in] gravity Gravity of the exception
     * @param[in] filename File name in which the exception has occurred
     * @param[in] line Line number
     * @param[in] function Function in which the exception has occurred
     * @param[in] code Error code for this exception
     * @param[in] arguments Additional arguments for the exception
     */
    template<typename... ARGUMENTS>
    XTracker(const severity_e severity,
             const std::string& fileName,
             const unsigned int line,
             const std::string& function,
             const error_code_t code,
             const ARGUMENTS... arguments) :
            XException<XTracker>(severity, fileName, line, function, code, arguments...)
    {}

    /// @see XException::decoder()
    [[maybe_unused]]
    static std::string decoder(error_code_t code)
    {
        switch (code)
        {
            case ERR_GPU_INVALID_DEVICE:
            case ERR_GPU_NOT_FOUND:
            case ERR_GPU_NOT_SUPPORTED:
            case ERR_GPU_STREAM_FAILURE:
                return "GPU Error: {}";
            case ERR_P1_DOWNSIZE:
            case ERR_P1_GAUSSIAN_SMOOTH:
            case ERR_P4_DOWNSIZE:
            case ERR_P4_GAUSSIAN_SMOOTH:
            case ERR_NPP_RESIZE:
            case ERR_NPP_SQRDISTANCE:
            case ERR_NPP_MIN_INDEX:
                return "NPP Error: {}";
            case ERR_P1_CENTER_OF_MASS:
                return "P1 COM Error: {}";
            case ERR_P1_LOCALIZE:
                return "P1 Localization Error: {}";
            case ERR_P4_ROI_FAILURE:
                return "P4 ROI Error: {}";
            case ERR_P4_LOCALIZE:
                return "P4 Localization Error: {}";
            case ERR_TRACKER_SETUP_FAILURE:
                return "DDPI Error: {}";
            default:
                return "Unkown Eroor";
        }
    }
}; // XTracker


/**
* @brief The XFrameGrabber exception manages all the exceptions relative to the frame grabber.
*/
class [[maybe_unused]] XGStreamer : XException<XGStreamer>
{
public:
    enum {
        GPU_ERROR,
        GST_ERROR,
    };

    /**
     * @brief Default constructor.
     *
     * @param[in] gravity Gravity of the exception
     * @param[in] filename File name in which the exception has occurred
     * @param[in] line Line number
     * @param[in] function Function in which the exception has occurred
     * @param[in] code Error code for this exception
     * @param[in] arguments Additional arguments for the exception
     */
    template<typename... ARGUMENTS>
    XGStreamer(const severity_e severity,
               const std::string& fileName,
               const unsigned int line,
               const std::string& function,
               const error_code_t code,
               const ARGUMENTS... arguments) :
            XException<XGStreamer>(severity, fileName, line, function, code, arguments...)
    {}

    /// @see XException::decoder()
    [[maybe_unused]]
    static std::string decoder(error_code_t code)
    {
        switch (code)
        {
            case GPU_ERROR:
                return "Cuda Error: {}";
            case GST_ERROR:
                return "Gstreamer Error: {}";
            default: return "";
        }
    }
};
