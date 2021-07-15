#include "../directx12lib.h"

#include "directx12descriptorheap.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12DescriptorHeap::DirectX12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) : mType(type)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        desc.Type = type;
        desc.NodeMask = 0;
        desc.NumDescriptors = 100;
        desc.Flags = type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
           
        HRESULT hr = sDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeap));
        DX12_CHECK(hr);
    }

    DirectX12DescriptorHeap::~DirectX12DescriptorHeap()
    {
        if (mHeap) {
            mHeap->Release();
            mHeap = nullptr;
        }
    }

    DirectX12DescriptorHeap& DirectX12DescriptorHeap::operator=(DirectX12DescriptorHeap&& other) {
        std::swap(mHeap, other.mHeap);
        std::swap(mIndex, other.mIndex);
        std::swap(mType, other.mType);
        return *this;
    }

    OffsetPtr DirectX12DescriptorHeap::allocate()
    {
        UINT handleSize = sDevice->GetDescriptorHandleIncrementSize(mType);

        assert(mIndex < 100);

        size_t index = mIndex;

        ++mIndex;

        return OffsetPtr{ index * handleSize };
    }

    void DirectX12DescriptorHeap::deallocate(OffsetPtr handle)
    {
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::cpuHandle(OffsetPtr index)
    {
        return mHeap->GetCPUDescriptorHandleForHeapStart() + index;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::gpuHandle(OffsetPtr index)
    {
        return mHeap->GetGPUDescriptorHandleForHeapStart() + index;
    }

    OffsetPtr DirectX12DescriptorHeap::fromGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        return handle - mHeap->GetGPUDescriptorHandleForHeapStart();
    }

    ID3D12DescriptorHeap *DirectX12DescriptorHeap::resource() const
    {
        return mHeap;
    }
}
}