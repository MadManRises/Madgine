#pragma once

#include "Modules/math/vector2i.h"

namespace Engine {
namespace Render {

	struct Glyph {
        Vector2i mSize;
        Vector2i mUV;
        int mBearingY;
        int mAdvance;
        bool mFlipped;
    };

}
}