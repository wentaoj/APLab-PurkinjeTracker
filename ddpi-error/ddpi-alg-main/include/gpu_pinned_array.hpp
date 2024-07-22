#pragma once
#include "gpu_array.hpp"
#include <cstring>

namespace ddpi {
    template<class T>
    class GpuPinnedArray : public GpuArray<T> {
    public:
        /** GpuPinnedArray Constructor.
         */
        GpuPinnedArray() : GpuArray<T>() {
            _data = nullptr;
        };

        GpuPinnedArray(const GpuPinnedArray<T> &) = delete;

        /** GpuPinnedArray Destructor.
         */
        ~GpuPinnedArray() {};

        T operator[](int idx) {
            // Check gpuarray has been alloated or assigned
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated or assigned");
            }

            if (idx >= GpuArray<T>::_size | idx < 0) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                                 "index is invalid");
            }

            return _data[idx];
        }


        /** Copy this GpuArray to another GpuArray.
         *  @param obj The GpuArray wiil be copied to
         */
        virtual void copyTo(GpuPinnedArray<T> &obj) final {
            // Check input pointer
            if (obj.empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_OPERATION_INVALID_ARGUMENT,
                                 "Input gpuobject is empty");
            }

            // Check gpuarray has been alloated or assigned
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated or assigned");
            }

            GpuArray<T>::copyTo(obj);
            obj.download();

            return;
        }

        void copyTo(T *ptr) {
            // Check gpuarray has been alloated or assigned
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated or assigned");
            }

            std::memcpy(ptr, _data, GpuArray<T>::_sizeInBytes);
        }

        /** Allocate this GpuArray in GPU.
         *  @param width The width this GpuArray
         *  @param height The height this GpuArray (Default = 1)
         */
        virtual void create(int size) final {
            GpuArray<T>::create(size);

            cudaError err = cudaHostAlloc(&_data, GpuArray<T>::_sizeInBytes, cudaHostAllocDefault);
            if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_CUDA_HOST_MALLOC,
                                 cudaGetErrorString(err));
            };
            memset(_data, 0, GpuArray<T>::_sizeInBytes);

            return;
        }


        void download(T *data) = delete;

        /** Copy from GPU to host memory.
        */
        void download() {
            // Check gpuarray has been alloated
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated");
            }

            // copy to dst
            cudaError err = cudaMemcpy(
                    _data,
                    GpuArray<T>::_gdata,
                    GpuArray<T>::_sizeInBytes,
                    cudaMemcpyDeviceToHost);
            if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                                 cudaGetErrorString(err));
            };

            //cudaDeviceSynchronize();
            return;
        }


        void downloandASync(T *data, cudaStream_t &stream) = delete;

        /** Copy GpuArray from GPU to host memory.
         */
        void downloadAsync(cudaStream_t &stream) {
            // Check gpuarray has been alloated
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated");
            }

            // copy to dst
            cudaError err = cudaMemcpyAsync(
                    _data,
                    GpuArray<T>::_gdata,
                    GpuArray<T>::_sizeInBytes,
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

        /** Release GpuArray from GPU.
         */
        virtual void release() {
            if (GpuArray<T>::empty()) return;

            GpuArray<T>::release();

            // copy to dst
            cudaError err = cudaFreeHost(_data);
            if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_CUDA_HOST_FREE,
                                 cudaGetErrorString(err));
            };
            _data = nullptr;

            return;
        }

        void upload(T *data, bool RDMA = false) = delete;

        virtual void upload() final {
            // Check gpuarray has been alloated
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated");
            }

            // Copy from cpu to gpu
            cudaError err = cudaMemcpy(
                    (void *) GpuArray<T>::_gdata,
                    (void *) _data,
                    GpuArray<T>::_sizeInBytes,
                    cudaMemcpyHostToDevice);
            if (err != cudaSuccess) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_CUDA_COPY,
                                 cudaGetErrorString(err));
            };
            return;
        }

        void uploadAsync(T *data, cudaStream_t &stream, bool RDMA = false) = delete;

        void uploadAsync(cudaStream_t &stream) {
            // Check gpuarray has been alloated
            if (GpuArray<T>::empty()) {
                THROW_ERROR_ARGS(XGpuObject,
                                 XGpuObject::ERR_GPUOBJECT_NOT_ALLOCATED,
                                 "Gpuobject is not allocated");
            }

            // Copy from CPU to GPU
            cudaError err = cudaMemcpyAsync(
                    (void *) GpuArray<T>::_gdata,
                    (void *) _data,
                    GpuArray<T>::_sizeInBytes,
                    cudaMemcpyHostToDevice,
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

    private:
        T *_data;
    };
} // source::ddpi