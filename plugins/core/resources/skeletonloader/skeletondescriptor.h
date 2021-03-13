#pragma once

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    struct Bone {
        std::string mName;
        Matrix4 mOffsetMatrix;
        Matrix4 mTTransform;
        IndexType<uint32_t> mParent;
        IndexType<uint32_t> mFirstChild;
    };

    struct SkeletonDescriptor {
        std::vector<Bone> mBones;     
        Matrix4 mMatrix;
    };

}
}