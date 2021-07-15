#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12DescriptorHeap
    {
        DirectX12DescriptorHeap() = default;
        DirectX12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
        ~DirectX12DescriptorHeap();

        DirectX12DescriptorHeap &operator=(DirectX12DescriptorHeap &&other);

        OffsetPtr allocate();
        void deallocate(OffsetPtr handle);

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle(OffsetPtr index);
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle(OffsetPtr index);
        OffsetPtr fromGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);

        ID3D12DescriptorHeap *resource() const;

    private:
        D3D12_DESCRIPTOR_HEAP_TYPE mType;
        ID3D12DescriptorHeap *mHeap = nullptr;
        size_t mIndex = 0;
    };

}
}