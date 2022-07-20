#pragma once

#include "Madgine/render/texturedescriptor.h"

namespace Engine {
namespace Render {

    struct RenderTextureConfig {
        bool mCreateDepthBufferView = false;
        size_t mSamples = 1;
        size_t mTextureCount = 1;
        size_t mIterations = 1;
        TextureType mType = TextureType_2D;
        DataFormat mFormat = FORMAT_RGBA8;
        std::string mName;
    };

}
}