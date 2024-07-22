#pragma once
#include <npp.h>
#include "helper_cuda.hpp"
// #include "x_ddpi.hpp"

namespace ddpi {
template <class T>
class Gpu2DArray {
public:
    /** GpuArray Constructor.
     */
    Gpu2DArray() {
        _gdata = nullptr;
        _width = 0;
        _widthInBytes = 0;
        _height = 0;
        _pitch = 0;
        _size = 0;
        _blocks = 0;
        _threads = 0;
    };
    Gpu2DArray(const Gpu2DArray<T> &) = delete;

    /** GpuArray Destructor.
     *  Virtual to allow for subclassing
     */
    virtual ~Gpu2DArray() {};

    /** Copy this GpuArray to another GpuArray.
     *  @param obj The GpuArray wiil be copied to
     */
    virtual void copyTo(Gpu2DArray<T>& obj) const {
        // Check input pointer
        if (obj.empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input is empy");
        }

        // Check gpuarray has been alloated or assigned
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated or assigned");
        }

        if ((obj._width != _width) | (obj._widthInBytes != _widthInBytes) | (obj._height != _height)) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject dimension not match");
        }

        // copy to dst
        cudaError err = cudaMemcpy2D(
                obj.gdata(), _pitch,
                _gdata, _pitch,
                _widthInBytes, _height,
                cudaMemcpyDeviceToDevice);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
            //         cudaGetErrorString(err));
        };

        return;
    }

    /** Copy this GpuArray to another GpuArray.
 *  @param obj The GpuArray wiil be copied to
 */
    virtual void copyToAsync(Gpu2DArray<T>& obj, cudaStream_t &stream) const {
        // Check input pointer
        if (obj.empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input is empy");
        }

        // Check gpuarray has been alloated or assigned
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated or assigned");
        }

        if ((obj._width != _width) | (obj._widthInBytes != _widthInBytes) | (obj._height != _height)) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject dimension not match");
        }

        // copy to dst
        cudaError err = cudaMemcpy2DAsync(
                obj.gdata(), _pitch,
                _gdata, _pitch,
                _widthInBytes, _height,
                cudaMemcpyDeviceToDevice,
                stream);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
            //         cudaGetErrorString(err));
        };

        return;
    }

    /** Allocate this GpuArray in GPU.
     *  @param width The width this GpuArray
     *  @param height The height this GpuArray (Default = 1)
     */
    virtual void create(int width, int height) {
        int size = width * height;
        if (size <= 0) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_DIMENSION,
            //         "The dimension of GPUObject is invalid");
        }
        _size = size;

        // Release memory before allocate
        release();

        // Allocate memory
        cudaError err = cudaMallocPitch((void**)&_gdata, &_pitch, width * sizeof(T), height);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_MALLOC,
            //         cudaGetErrorString(err));
        };

        // Set the dimension of memory
        _width = width;
        _widthInBytes = width * sizeof(T);
        _height = height;
        _paddedWidth = _pitch / sizeof(T);
        return;
    }

    /** Get raw gpu memory pointer
     *  @return Raw gpu memory pointer of this GpuArray
     */
    T* gdata() { return _gdata; }

    /** Copy GpuArray from GPU to host memory.
     *  @param data The pointer in host memory
     */
    virtual void download(T* data) {
        // Check input pointer
        if (data == nullptr) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input pointer is null");
        }

        // Check gpuarray has been alloated
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated");
        }

        // copy to dst
        cudaError err = cudaMemcpy2D(
                data, _widthInBytes,
                _gdata, _pitch,
                _widthInBytes, _height,
                cudaMemcpyDeviceToHost);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
            //         cudaGetErrorString(err));
        };

        return;
    }

    /** Copy GpuArray from GPU to host memory.
     *  @param data The pointer in host memory
     */
    virtual void downloadAsync(T* data, cudaStream_t &stream) {
        // Check input pointer
        if (data == nullptr) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input is null");
        }

        // Check gpuarray has been alloated
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated");
        }

        // copy to dst
        cudaError err = cudaMemcpy2DAsync(
                data, _widthInBytes,
                _gdata, _pitch,
                _widthInBytes, _height,
                cudaMemcpyDeviceToHost,
                stream);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
            //         cudaGetErrorString(err));
        };

        return;
    }

    /** Check GpuArray is allocated.
     *  @return true if GpuArray is not allocated
     */
    bool empty() const {
        return _gdata == nullptr;
    }

    /** Copy GpuArray from GPU to host memory.
     *  @param data The pointer in host memory
     */
    int height() const { return _height; }

    /** Release GpuArray from GPU.
     */
    virtual void release() {
        if (empty()) return;

        // Release memory
        cudaError err = cudaFree(_gdata);
        if (err != cudaSuccess) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_CUDA_FREE,
            //         cudaGetErrorString(err));
        };
        _gdata = nullptr;

        // Reset the dimension of memory
        _width = 0;
        _height = 0;
        _pitch = 0;
        return;
    }

    int paddedWidth() const
    { return _paddedWidth; }

    int pitch() const { return _pitch; }

    virtual void upload(T* data, bool RDMA=false) {
        // Check input pointer
        if (data == nullptr) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input is null");
        }

        // Check gpuarray has been alloated
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated");
        }

        // Copy to dst
        if (RDMA) {
            // From GPU to GPU
            cudaError err = cudaMemcpy2D(
                    _gdata, _pitch,
                    data, _pitch,
                    _widthInBytes, _height,
                    cudaMemcpyDeviceToDevice);
            if (err != cudaSuccess) {
                // THROW_ERROR_ARGS(
                //         XGpuObject,
                //         XGpuObject::ERR_GPUOBJECT_CUDA_FREE,
                //         cudaGetErrorString(err));
            };
        }
        else {
            // From CPU to GPU
            cudaError err = cudaMemcpy2D(
                    _gdata, _pitch,
                    data, _pitch,
                    _widthInBytes, _height,
                    cudaMemcpyHostToDevice);
            if (err != cudaSuccess) {
                // THROW_ERROR_ARGS(
                //         XGpuObject,
                //         XGpuObject::ERR_GPUOBJECT_CUDA_FREE,
                //         cudaGetErrorString(err));
            };
        }
        return;
    }

    virtual void uploadAsync(T* data, cudaStream_t &stream, bool RDMA = false) {
        // Check input pointer
        if (data == nullptr) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
            //         "Input is null");
        }

        // Check gpuarray has been alloated
        if (empty()) {
            // THROW_ERROR_ARGS(
            //         XGpuObject,
            //         XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
            //         "GPUObject is not allocated");
        }

        // Copy to dst
        if (RDMA) {
            // From GPU to GPU
            cudaError err = cudaMemcpy2DAsync(
                    _gdata, _pitch,
                    data, _widthInBytes,
                    _widthInBytes, _height,
                    cudaMemcpyDeviceToDevice,
                    stream);
            if (err != cudaSuccess) {
                // THROW_ERROR_ARGS(
                //         XGpuObject,
                //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                //         cudaGetErrorString(err));
            };
        }
        else {
            // From CPU to GPU
            cudaError err = cudaMemcpy2DAsync(
                    _gdata, _pitch,
                    data, _widthInBytes,
                    _widthInBytes, _height,
                    cudaMemcpyHostToDevice,
                    stream);
            if (err != cudaSuccess) {
                // THROW_ERROR_ARGS(
                //         XGpuObject,
                //         XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                //         cudaGetErrorString(err));
            };
        }
        return;
    }

    int width() const { return _width; }

    size_t widthInBytes() const { return _widthInBytes; }

    int blocks() {
        return _blocks;
    }

    int threads() {
        return _threads;
    }

    void setBlocksAndThreads(int block, int thread) {
        _blocks = block;
        _threads = thread;
    }

protected:
    T* _gdata;
    int _height;
    size_t _pitch;
    int _paddedWidth;
    int _size;
    int _width;
    size_t _widthInBytes;
    int _blocks;
    int _threads;

};

}; 