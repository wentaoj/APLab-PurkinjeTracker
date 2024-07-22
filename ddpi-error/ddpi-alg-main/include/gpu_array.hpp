#pragma once
#include <npp.h>
#include "helper_cuda.hpp"
#include "x_ddpi.hpp"

namespace ddpi {

template <class T>
class GpuArray {
public:
	/** GpuArray Constructor.
	 */
	GpuArray() {
        _gdata = nullptr;
        _size = 0;
        _sizeInBytes = 0;
        _blocks = 0;
        _threads = 0;
	};
	GpuArray(const GpuArray<T> &) = delete;

	/** GpuArray Destructor.
	 *  Virtual to allow for subclassing
	 */
	virtual ~GpuArray() {};

	/** Copy this GpuArray to another GpuArray.
	 *  @param obj The GpuArray wiil be copied to
	 */
	virtual void copyTo(GpuArray<T>& obj) const {
		// Check input pointer
		if (obj.empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input gpuobject is empty");
		}

		// Check gpuarray has been alloated or assigned
		if (empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                             XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "Gpuobject is not allocated or assigned");
		}

		// copy to dst
		cudaError err = cudaMemcpy(
			(void*) obj.gdata(),
			(void*) _gdata,
			_sizeInBytes,
			cudaMemcpyDeviceToDevice);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};

		return;
	}

	/** Allocate this GpuArray in GPU.
	 *  @param width The width this GpuArray
	 *  @param height The height this GpuArray (Default = 1)
	 */
	virtual void create(int size) {
		if (size <= 0) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_DIMENSION,
                    "The dimension of gpuobject is invalid");
		}

		// Release memory before allocate
		release();

		// Allocate memory
		cudaError err = cudaMalloc((void**)&_gdata, size * sizeof(T));
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_MALLOC,
                    cudaGetErrorString(err));
		};


		// Set the dimension of memory
		_size = size;
		_sizeInBytes = size * sizeof(T);
		return;
	}

	/** Get raw gpu memory pointer
	 *  @return Raw gpu memory pointer of this GpuArray
	 */
	T* gdata() { return _gdata; }

	/** Copy GpuArray from GPU to host memory.
	 *  @param data The pointer in host memory
	 */
	void download(T* data) {
		// Check input pointer
		if (data == nullptr) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input pointer is null");
		}

		// Check gpuarray has been alloated
		if (empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "Gpuobject is not allocated");
		}

		// copy to dst
		cudaError err = cudaMemcpy(
			data,
			_gdata,
			_sizeInBytes,
			cudaMemcpyDeviceToHost);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};

		return;
	}

	/** Copy GpuArray from GPU to host memory.
	 *  @param data The pointer in host memory
	 */
	void downloadAsync(T* data, cudaStream_t &stream) {
		// Check input pointer
		if (data == nullptr) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input pointer is null");
		}

		// Check gpuarray has been alloated
		if (empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "Gpuobject is not allocated");
		}

		// copy to dst
		cudaError err = cudaMemcpyAsync(
			data,
			_gdata,
			_sizeInBytes,
			cudaMemcpyDeviceToHost,
			stream);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
		};

        err = cudaStreamSynchronize(stream);
        if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
        };

		return;
	}

	/** Check GpuArray is allocated.
	 *  @return true if GpuArray is not allocated
	 */
	bool empty() const {
		return _gdata == nullptr;
	}

	/** Release GpuArray from GPU.
	 */
	virtual void release() {
		if (empty()) return;

		// Release memory
		cudaError err = cudaFree(_gdata);
		if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_FREE,
                    cudaGetErrorString(err));
		};
		_gdata = nullptr;

		// Reset the dimension of memory
		_size = 0;
		_sizeInBytes = 0;
		return;
	}

	int size() const { return _size; }

	size_t sizeInBytes() const { return _sizeInBytes; }

	void upload(T* data, bool RDMA=false) {
		// Check input pointer
		if (data == nullptr) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input data is null");
		}

		// Check gpuarray has been alloated
		if (empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "Gpuobject is not allocated");
		}

		// Copy to dst
		if (RDMA) {
			// From GPU to GPU
			cudaError err = cudaMemcpy(
				(void*)_gdata,
				(void*)data,
				_sizeInBytes,
				cudaMemcpyDeviceToDevice);
			if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                        XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                        cudaGetErrorString(err));
			};
		}
		else {
			// From CPU to GPU
			cudaError err = cudaMemcpy(
				_gdata,
				data,
				_sizeInBytes,
				cudaMemcpyHostToDevice);
			if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                        XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                        cudaGetErrorString(err));
			};
		}
		return;
	}

	void uploadAsync(T* data, cudaStream_t &stream, bool RDMA = false) {
		// Check input pointer
		if (data == nullptr) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                    "Input data is null");
		}

		// Check gpuarray has been alloated
		if (empty()) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                    "Gpuobject is not allocated");
		}

		// Copy to dst
		if (RDMA) {
			// From GPU to GPU
			cudaError err = cudaMemcpyAsync(
				(void*)_gdata,
				(void*)data,
				_sizeInBytes,
				cudaMemcpyDeviceToDevice,
				stream);
			if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                        XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                        cudaGetErrorString(err));
			};
		}
		else {
			// From CPU to GPU
			cudaError err = cudaMemcpyAsync(
				(void*)_gdata,
				(void*)data,
				_sizeInBytes,
				cudaMemcpyHostToDevice,
				stream);
			if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                        XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                        cudaGetErrorString(err));
			};
		}

        cudaError err = cudaStreamSynchronize(stream);
        if (err != cudaSuccess) {
            THROW_ERROR_ARGS(XGpuObject,
                    XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                    cudaGetErrorString(err));
        };

		return;
	}

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
	int _size;
	size_t _sizeInBytes;
	int _blocks;
	int _threads;
};

} 