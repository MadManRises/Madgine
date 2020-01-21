#pragma once

#include "font.h"
#include "util/directx11texture.h"

namespace Engine {
namespace Render {

    struct DirectX11FontData : Font::Font {
        DirectX11Texture mTexture;
    };

}
}