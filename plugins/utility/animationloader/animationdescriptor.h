#pragma once

#include "Modules/math/quaternion.h"

#include "skeletonloader.h"

namespace Engine {
namespace Render {

    template <typename T>
    struct KeyFrame : ScopeBase {

        KeyFrame(float time, T value)
            : mTime(time)
            , mValue(value)
        {
        }

        float mTime;
        T mValue;
    };

    using VectorKeyFrame = KeyFrame<Vector3>;
    using QuaternionKeyFrame = KeyFrame<Quaternion>;

    struct AnimationBone : ScopeBase {
        std::vector<VectorKeyFrame> mPositions;
        std::vector<QuaternionKeyFrame> mOrientations;
        std::vector<VectorKeyFrame> mScalings;
    };

    struct AnimationDescriptor : ScopeBase {
        std::string mName;
        float mDuration;
        float mTicksPerSecond;

        std::vector<AnimationBone> mBones;
    };

    struct MADGINE_ANIMATIONLOADER_EXPORT AnimationList : ScopeBase {

		AnimationList(const AnimationList &) = delete;
        AnimationList(AnimationList &&) = default;

        std::map<std::string, AnimationDescriptor> mAnimations;
        std::vector<std::string> mBoneNames;

		std::map<SkeletonLoader::HandleType, std::unique_ptr<int[]>> mBoneMappings;
        int *generateBoneMappings(const SkeletonLoader::SkeletonLoader::HandleType &handle);
    };

}
}