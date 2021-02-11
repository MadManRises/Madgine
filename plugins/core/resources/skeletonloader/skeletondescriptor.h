#pragma once

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    struct Bone {
        std::string mName;
        Matrix4 mOffsetMatrix;
        Matrix4 mTTransform;
        uint32_t mParent = std::numeric_limits<uint32_t>::max();
        uint32_t mFirstChild = std::numeric_limits<uint32_t>::max();
    };

    struct SkeletonDescriptor {
        std::vector<Bone> mBones;     
        Matrix4 mMatrix;
    };

}
}