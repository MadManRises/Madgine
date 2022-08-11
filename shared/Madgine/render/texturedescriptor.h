#pragma once

namespace Engine {
namespace Render {

    enum TextureType {
        TextureType_2D,
        TextureType_2DMultiSample,
        TextureType_Cube
    };

    enum DataFormat {
        FORMAT_RGBA8,
        FORMAT_RGBA16F,
        FORMAT_R32F,
        FORMAT_D24,
        FORMAT_D32
    };

    struct TextureDescriptor {
        TextureHandle mTextureHandle = 0;
        TextureType mType;

        auto operator<=>(const TextureDescriptor &) const = default;
    };

}
}