#pragma once

#include "Meta/math/quaternion.h"

#include "skeletonloader.h"

namespace Engine {
namespace Render {

    template <typename T>
    struct KeyFrame {
        float mTime;
        T mValue;
    };

    using VectorKeyFrame = KeyFrame<Vector3>;
    using QuaternionKeyFrame = KeyFrame<Quaternion>;

    struct AnimationBone {
        std::vector<VectorKeyFrame> mPositions;
        std::vector<QuaternionKeyFrame> mOrientations;
        std::vector<VectorKeyFrame> mScalings;
    };

    struct AnimationDescriptor {
        std::string mName;
        float mDuration;
        float mTicksPerSecond;

        std::vector<AnimationBone> mBones;
    };

    struct MADGINE_ANIMATIONLOADER_EXPORT AnimationList {
        AnimationList() = default;
		AnimationList(const AnimationList &) = delete;
        AnimationList(AnimationList &&) = default;

        std::map<std::string, AnimationDescriptor> mAnimations;
        std::vector<std::string> mBoneNames;

		std::map<SkeletonLoader::HandleType, std::unique_ptr<int[]>> mBoneMappings;
        int *generateBoneMappings(const SkeletonLoader::SkeletonLoader::HandleType &handle);
    };

}
}