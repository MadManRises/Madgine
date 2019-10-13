#pragma once

namespace Engine {
namespace Render {

    typedef int TextureFlags;
    enum TextureFlags_ {
        TextureFlag_IsDistanceField = 1 << 0
    };

    typedef uint32_t TextureHandle;
    struct TextureDescriptor {
        TextureHandle mHandle = 0;
        TextureFlags mFlags = 0;

        bool operator<(const TextureDescriptor &other) const
        {
            return mHandle < other.mHandle || (mHandle == other.mHandle && mFlags < other.mFlags);
        }
    };

}
}