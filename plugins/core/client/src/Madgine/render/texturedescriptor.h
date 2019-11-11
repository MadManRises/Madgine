#pragma once

#include "textureloader.h"

namespace Engine {
namespace Render {

    typedef int TextureFlags;
    enum TextureFlags_ {
        TextureFlag_IsDistanceField = 1 << 0
    };

    typedef uint32_t TextureHandle;
    struct TextureDescriptor {
        const Texture *mTexture;
        TextureFlags mFlags = 0;

        bool operator<(const TextureDescriptor &other) const
        {
            return mTexture < other.mTexture || (mTexture == other.mTexture && mFlags < other.mFlags);
        }
    };

}
}