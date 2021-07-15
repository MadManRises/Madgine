#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Buffer {

        DirectX11Buffer() = default;
        DirectX11Buffer(UINT bind, size_t size);
        DirectX11Buffer(UINT bind, const ByteBuffer &data);
        DirectX11Buffer(const DirectX11Buffer &) = delete;
        DirectX11Buffer(DirectX11Buffer &&);
        ~DirectX11Buffer();

        DirectX11Buffer &operator=(const DirectX11Buffer &) = delete;
        DirectX11Buffer &operator=(DirectX11Buffer &&);

        explicit operator bool() const;

        void bindVertex(UINT stride) const;
        void bindIndex() const;

        void reset();
        void setData(const ByteBuffer &data);
        void resize(size_t size);
        WritableByteBuffer mapData();

        ID3D11Buffer *handle();

    private:
        size_t mSize = 0;
        UINT mBind = 0;
        ID3D11Buffer *mBuffer = nullptr;                
    };

}
}