#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12Buffer {

        DirectX12Buffer() = default;
        DirectX12Buffer(const ByteBuffer &data);
        DirectX12Buffer(const DirectX12Buffer &) = delete;
        DirectX12Buffer(DirectX12Buffer &&);
        ~DirectX12Buffer();

        DirectX12Buffer &operator=(const DirectX12Buffer &) = delete;
        DirectX12Buffer &operator=(DirectX12Buffer &&);

        explicit operator bool() const;

        void bindVertex(ID3D12GraphicsCommandList *commandList, UINT stride, size_t index = 0) const;
        void bindIndex(ID3D12GraphicsCommandList *commandList) const;

        void reset(size_t size = 0);
        void setData(const ByteBuffer &data);
        

        WritableByteBuffer mapData(size_t size);
        WritableByteBuffer mapData();

        //D3D12_GPU_DESCRIPTOR_HANDLE handle();

        D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() const;

    private:
        D3D12_GPU_VIRTUAL_ADDRESS mAddress = 0;
        size_t mSize = 0;
        bool mIsPersistent;
    };

}
}