#pragma once
#include "gpu_2darray.hpp"
#include <cstring>

namespace ddpi {
template <class T>
class GpuPinned2DArray : public Gpu2DArray<T> {
public:
	/** GpuPinned2DArray Constructor.
	 */
	GpuPinned2DArray() : Gpu2DArray<T>() {
		_data = nullptr;
        _size = 0;
        _sizeInBytes = 0;
	};
	GpuPinned2DArray(const GpuPinned2DArray<T>&) = delete;

	/** GpuPinned2DArray Destructor.
	 */
	~GpuPinned2DArray() {};

	T operator [] (int idx) {
		// Check Gpu2DArray has been alloated or assigned
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}
		
		if (idx >= _size | idx < 0) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Index is invalid");
		}

		return _data[idx];
	}


	/** Copy this Gpu2DArray to another Gpu2DArray.
	 *  @param obj The Gpu2DArray wiil be copied to 
	 */
	virtual void copyTo(GpuPinned2DArray<T>& obj) final {
		// Check input pointer
		if (obj.empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input is null");
		}

		// Check Gpu2DArray has been alloated or assigned
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

		Gpu2DArray<T>::copyTo(obj);
		obj.download();

		return;
	}

    /** Copy this Gpu2DArray to another Gpu2DArray.
     *  @param obj The Gpu2DArray wiil be copied to
     */
    virtual void copyToAsync(GpuPinned2DArray<T>& obj) final {
        // Check input pointer
        if (obj.empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input is null");
        }

        // Check Gpu2DArray has been alloated or assigned
        if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
        }

        Gpu2DArray<T>::copyToAsync(obj);
        obj.download();

        return;
    }

	void copyToAsync(T* ptr) {
		// Check Gpu2DArray has been alloated or assigned
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

		std::memcpy(ptr, _data, _sizeInBytes);
	}

	/** Allocate this Gpu2DArray in GPU.
	 *  @param width The width this Gpu2DArray
	 *  @param height The height this Gpu2DArray (Default = 1)
	 */
	virtual void create(int width, int height) final {
		Gpu2DArray<T>::create(width, height);
        _size = _height * _width;
		_sizeInBytes = _size * sizeof(T);
		
		cudaError err = cudaHostAlloc(&_data, _sizeInBytes, cudaHostAllocDefault);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_HOST_MALLOC,
                    cudaGetErrorString(err));
		};
		return;
	}


    virtual void download(T* data) = delete;
	
	/** Copy from GPU to host memory.
	*/
	void download()
	{
		// Check Gpu2DArray has been alloated
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

		// copy to dst
		cudaError err = cudaMemcpy2D(
                _data, _widthInBytes,
                _gdata, _pitch,
                _widthInBytes, _height,2D
                cudaMemcpyDeviceToHost);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};

		//cudaDeviceSynchronize();
		return;
	}


    virtual void downloandASync(T* data, cudaStream_t &stream) = delete;
	
	/** Copy Gpu2DArray from GPU to host memory.
	 */
	void downloadASync(cudaStream_t &stream)
	{
		// Check Gpu2DArray has been alloated
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

        // copy to dst
        cudaError err = cudaMemcpy2DAsync(
                _data, _widthInBytes,
                _gdata, _pitch,
                _widthInBytes, _height,
                cudaMemcpyDeviceToHost,
                stream);
        if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
        };

		return;
	}

	/** Release Gpu2DArray from GPU.
	 */
	virtual void release() {
		if (Gpu2DArray<T>::empty()) return;
			
		Gpu2DArray<T>::release();

		// copy to dst
		cudaError err = cudaFreeHost(_data);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_HOST_FREE,
                    cudaGetErrorString(err));
		};
		_data = nullptr;

		return;
	}

	void upload(T* data, bool RDMA = false) = delete;

	virtual void upload() final {
		// Check Gpu2DArray has been alloated
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

		// Copy from cpu to gpu
		cudaError err = cudaMemcpy2D(
                Gpu2DArray<T>::_gdata, _pitch,
                _data, _widthInBytes,
                _widthInBytes, _height,
                cudaMemcpyHostToDevice);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};
		return;
	}

	void uploadAsync(T* data, cudaStream_t &stream, bool RDMA = false) = delete;

	void uploadAsync(cudaStream_t &stream) {
		// Check Gpu2DArray has been alloated
		if (Gpu2DArray<T>::empty()) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "GPUObject is not allocated or assigned");
		}

		// Copy from CPU to GPU
		cudaError err = cudaMemcpy2DAsync(
                Gpu2DArray<T>::_gdata, _pitch,
                _data, _widthInBytes,
                _widthInBytes, _height,
                cudaMemcpyHostToDevice,
                stream);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(
                    XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};
		return;
	}

private:
	T* _data;
	int _size;
	size_t _sizeInBytes;
};
}; // ddpi