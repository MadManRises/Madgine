#pragma once

#include "Modules/math/matrix4.h"

namespace Engine {
namespace Render {

    struct Bone : ScopeBase {
        Bone(std::string name, Matrix4 offsetMatrix)
            : mName(std::move(name))
            , mOffsetMatrix(offsetMatrix)            
        {
        }

        std::string mName;
        Matrix4 mOffsetMatrix;
        Matrix4 mTTransform;
        size_t mParent = std::numeric_limits<size_t>::max();
        size_t mFirstChild = std::numeric_limits<size_t>::max();
    };

    struct SkeletonDescriptor : ScopeBase {
        std::vector<Bone> mBones;
        Matrix4 mBaseTransform;
    };

}
}