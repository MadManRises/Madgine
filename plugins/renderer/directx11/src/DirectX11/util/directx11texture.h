#pragma once

#include "Modules/math/vector2i.h"

#include "texture.h"

#include "textureloader.h"

namespace Engine {
namespace Render {

    enum TextureType {
        Texture2D
    };

    struct MADGINE_DIRECTX11_EXPORT DirectX11Texture : Texture {

        DirectX11Texture(TextureType type, DataFormat format, UINT bind, size_t width, size_t height, const void *data = nullptr);
        DirectX11Texture(TextureType type = Texture2D, DataFormat format = FORMAT_FLOAT8, UINT bind = D3D11_BIND_SHADER_RESOURCE);
        DirectX11Texture(const DirectX11Texture &) = delete;
        DirectX11Texture(DirectX11Texture &&);
        ~DirectX11Texture();

        DirectX11Texture &operator=(DirectX11Texture &&);

        void reset();

        void bind() const;

        void setData(Vector2i size, void *data);
        void setSubData(Vector2i offset, Vector2i size, void *data);

        void resize(Vector2i size);

        TextureHandle handle() const;
        ID3D11Resource *resource() const;

        /*void setWrapMode(GLint mode);
        void setFilter(GLint filter);*/

    private:
        ID3D11Resource *mResource = nullptr;
        ID3D11SamplerState *mSampler = nullptr;
        TextureType mType;
        Vector2i mSize = { 0, 0 };
        DataFormat mFormat;
        UINT mBind;
    };

}
}