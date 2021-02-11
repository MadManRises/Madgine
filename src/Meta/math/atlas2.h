#pragma once

#include "vector2i.h"
#include "rect2i.h"

namespace Engine {

struct META_EXPORT Atlas2 {

    struct Entry {
        Rect2i mArea;
        bool mFlipped;
    };

	struct Bin {
        Vector2i mOrigin;
        std::vector<Vector2i> mCorners; //Ordered top -> down ( x left -> right)
    };

    Atlas2(const Vector2i &binSize)
        : mBinSize(binSize)
    {
    }

    void addBin(const Vector2i &origin);

	Entry insert(const Vector2i &size, const std::function<void()> &expand, bool allowFlip = true);
    std::vector<Entry> insert(const std::vector<Vector2i> &sizes, const std::function<void()> &expand, bool allowFlip = true);


	const Vector2i mBinSize;

private:

    std::vector<Bin> mBins;
};

}