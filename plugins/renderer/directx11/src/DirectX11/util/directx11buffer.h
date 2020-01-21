#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Buffer {

        DirectX11Buffer() = default;
        DirectX11Buffer(UINT bind, size_t size, const void *data);
        DirectX11Buffer(const DirectX11Buffer &) = delete;
        DirectX11Buffer(DirectX11Buffer &&);
        ~DirectX11Buffer();

        DirectX11Buffer &operator=(const DirectX11Buffer &) = delete;
        DirectX11Buffer &operator=(DirectX11Buffer &&);

        operator bool() const;

        void bindVertex(UINT stride) const;
        void bindIndex() const;

        void reset();
        void setData(size_t size, const void *data);

        ID3D11Buffer *handle();

    private:
        UINT mBind = 0;
        ID3D11Buffer *mBuffer = nullptr;        
    };

}
}