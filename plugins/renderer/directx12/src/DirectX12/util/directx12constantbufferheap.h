#pragma once

namespace Engine {
namespace Render {


struct DirectX12ConstantBufferHeap {
        DirectX12ConstantBufferHeap() = default;
        DirectX12ConstantBufferHeap(DirectX12DescriptorHeap *descriptorHeap, size_t size);
        ~DirectX12ConstantBufferHeap();

        DirectX12ConstantBufferHeap &operator=(DirectX12ConstantBufferHeap &&other);

        OffsetPtr allocate(size_t size);

        WritableByteBuffer map(OffsetPtr offset, size_t size);

        DirectX12DescriptorHeap *descriptorHeap() const;

        ID3D12Resource *resource() const;

        D3D12_GPU_VIRTUAL_ADDRESS address(OffsetPtr offset);

        private:
        DirectX12DescriptorHeap *mDescriptorHeap = nullptr;
        ID3D12Resource *mHeap = nullptr;
        size_t mSize = 0;
        OffsetPtr mOffset { 0 };
    };

}
}