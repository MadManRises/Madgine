#pragma once

#include "vector2i.h"
#include "rect2i.h"

namespace Engine {

struct META_EXPORT Atlas2 {

    struct Entry {
        Rect2i mArea;
        bool mFlipped;
    };

    Atlas2(const Vector2i &binSize)
        : mBinSize(binSize)
    {
    }

    void addBin(const Vector2i &origin);

	Entry insert(const Vector2i &size, const std::function<void()> &expand, bool allowFlip = true);
    std::vector<Entry> insert(const std::vector<Vector2i> &sizes, const std::function<void()> &expand, bool allowFlip = true);


private:

    struct Bin {
        Vector2i mOrigin;
        std::vector<Vector2i> mCorners; //Ordered top -> down ( x left -> right)
    };

    std::tuple<float, int, bool> getScore(Bin &bin, const Vector2i &size, bool allowFlip);
    static Atlas2::Entry insertIntoBin(Bin &bin, const Vector2i &_size, int cornerIndex, bool flipped);

    const Vector2i mBinSize;
    std::vector<Bin> mBins;
};

}