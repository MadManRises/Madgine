#pragma once

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    struct Bone {
        std::string mName;
        Matrix4 mOffsetMatrix;
        Matrix4 mTTransform;
        Matrix4 mPreTransform = Matrix4::IDENTITY;
        IndexType<uint32_t> mParent;
        IndexType<uint32_t> mFirstChild;
    };

    struct SkeletonDescriptor {
        std::vector<Bone> mBones;     
        Matrix4 mMatrix;
    };

}
}