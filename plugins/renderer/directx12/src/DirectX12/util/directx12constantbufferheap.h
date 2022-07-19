#pragma once

namespace Engine {
namespace Render {

    struct DirectX12ConstantBufferHeap {
        DirectX12ConstantBufferHeap() = default;
        DirectX12ConstantBufferHeap(size_t size);

        DirectX12ConstantBufferHeap &operator=(DirectX12ConstantBufferHeap &&other);

        D3D12_GPU_VIRTUAL_ADDRESS allocateTemp(size_t size);
        void deallocateTemp(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size);
        D3D12_GPU_VIRTUAL_ADDRESS allocatePersistent(size_t size);
        void deallocatePersistent(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size);

        WritableByteBuffer map(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size);
        void setData(D3D12_GPU_VIRTUAL_ADDRESS ptr, const ByteBuffer &data);

        ID3D12Resource *resourceTemp() const;
        ID3D12Resource *resourcePersistent() const;

    private:
        ReleasePtr<ID3D12Resource> mTempHeap;
        ReleasePtr<ID3D12Resource> mPersistentHeap;
        size_t mSize = 0;
        OffsetPtr mOffsetTemp { 0 };
        OffsetPtr mOffsetPersistent { 0 };

        //std::vector<std::pair<OffsetPtr, size_t>> mFreeListTemp;
        std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, size_t>> mFreeListPersistent;
    };

}
}