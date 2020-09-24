#pragma once

namespace Engine {
namespace Render {

    typedef int TextureFlags;
    enum TextureFlags_ {
        TextureFlag_IsDistanceField = 1 << 0
    };

    struct TextureDescriptor {
        TextureHandle mTextureHandle;
        TextureFlags mFlags = 0;

        bool operator<(const TextureDescriptor &other) const
        {
            return mTextureHandle < other.mTextureHandle || (mTextureHandle == other.mTextureHandle && mFlags < other.mFlags);
        }
    };

}
}