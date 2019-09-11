#pragma once

namespace Engine {
namespace Font {

	struct Glyph {
        Vector2i mSize;
        Vector2i mUV;
        int mBearingY;
        int mAdvance;
        bool mFlipped;
    };

}
}