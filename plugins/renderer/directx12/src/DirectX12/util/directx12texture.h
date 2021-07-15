#pragma once

#include "Meta/math/vector2i.h"

#include "texture.h"

#include "textureloader.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    enum TextureType {
        Texture2D,
        RenderTarget2D
    };

    struct MADGINE_DIRECTX12_EXPORT DirectX12Texture : Texture {

        DirectX12Texture(TextureType type, DataFormat format, size_t width, size_t height, const ByteBuffer &data = {});
        DirectX12Texture(TextureType type = Texture2D, DataFormat format = FORMAT_FLOAT8);
        DirectX12Texture(const DirectX12Texture &) = delete;
        DirectX12Texture(DirectX12Texture &&);
        ~DirectX12Texture();

        DirectX12Texture &operator=(DirectX12Texture &&);

        void reset();

        void bind() const;

        void setData(Vector2i size, const ByteBuffer &data);
        void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data);

        void resize(Vector2i size);

        ID3D12Resource *resource() const;

        /*void setWrapMode(GLint mode);
        void setFilter(GLint filter);*/

    private:
        ID3D12Resource *mResource = nullptr;
        TextureType mType;
        Vector2i mSize = { 0, 0 };
        DataFormat mFormat;        
    };

}
}