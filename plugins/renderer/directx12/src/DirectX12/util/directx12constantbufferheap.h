#pragma once

namespace Engine {
namespace Render {

    struct DirectX12ConstantBufferHeap {
        DirectX12ConstantBufferHeap() = default;
        DirectX12ConstantBufferHeap(DirectX12DescriptorHeap *descriptorHeap, size_t size);
        ~DirectX12ConstantBufferHeap();

        DirectX12ConstantBufferHeap &operator=(DirectX12ConstantBufferHeap &&other);

        OffsetPtr allocateTemp(size_t size);
        void deallocateTemp(OffsetPtr ptr, size_t size);
        OffsetPtr allocatePersistent(size_t size);
        void deallocatePersistent(OffsetPtr ptr, size_t size);

        WritableByteBuffer map(OffsetPtr offset, size_t size);
        void setData(OffsetPtr offset, const ByteBuffer &data);

        DirectX12DescriptorHeap *descriptorHeap() const;

        ID3D12Resource *resourceTemp() const;
        ID3D12Resource *resourcePersistent() const;

        D3D12_GPU_VIRTUAL_ADDRESS addressTemp(OffsetPtr offset);
        D3D12_GPU_VIRTUAL_ADDRESS addressPersistent(OffsetPtr offset);

    private:
        DirectX12DescriptorHeap *mDescriptorHeap = nullptr;
        ID3D12Resource *mTempHeap = nullptr;
        ID3D12Resource *mPersistentHeap = nullptr;
        size_t mSize = 0;
        OffsetPtr mOffsetTemp { 0 };
        OffsetPtr mOffsetPersistent { 0 };

        std::vector<std::pair<OffsetPtr, size_t>> mFreeListTemp;
        std::vector<std::pair<OffsetPtr, size_t>> mFreeListPersistent;
    };

}
}