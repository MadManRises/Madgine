#pragma once

#include "Meta/math/vector2i.h"

#include "texture.h"

#include "textureloader.h"

#include "Generic/bytebuffer.h"

#include "render/texturedescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Texture : Texture {

        DirectX11Texture(TextureType type, DataFormat format, UINT bind, size_t width, size_t height, size_t samples = 1, const ByteBuffer &data = {});
        DirectX11Texture(TextureType type = TextureType_2D, DataFormat format = FORMAT_RGBA8, UINT bind = D3D11_BIND_SHADER_RESOURCE, size_t samples = 1);
        DirectX11Texture(const DirectX11Texture &) = delete;
        DirectX11Texture(DirectX11Texture &&);
        ~DirectX11Texture();

        DirectX11Texture &operator=(DirectX11Texture &&);

        void reset();

        void bind() const;

        void setData(Vector2i size, const ByteBuffer &data);
        void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data);

        void resize(Vector2i size);

        ID3D11Resource *resource() const;

        TextureDescriptor descriptor() const;
        DataFormat format() const;
        TextureType type() const;

        /*void setWrapMode(GLint mode);
        void setFilter(GLint filter);*/

    private:
        ReleasePtr<ID3D11Resource> mResource;
        TextureType mType;
        Vector2i mSize = { 0, 0 };
        DataFormat mFormat;
        UINT mBind;
        size_t mSamples;
    };

}
}