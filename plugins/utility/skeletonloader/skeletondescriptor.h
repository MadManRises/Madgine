#pragma once

#include "Modules/math/matrix4.h"

namespace Engine {
namespace Render {

    struct Bone : ScopeBase {
        Bone(std::string name)
            : mName(std::move(name))
        {
        }

        std::string mName;
        Matrix4 mOffsetMatrix;
        Matrix4 mTTransform;
        uint32_t mParent = std::numeric_limits<uint32_t>::max();
        uint32_t mFirstChild = std::numeric_limits<uint32_t>::max();
    };

    struct SkeletonDescriptor : ScopeBase {
        std::vector<Bone> mBones;     
        Matrix4 mMatrix;
    };

}
}