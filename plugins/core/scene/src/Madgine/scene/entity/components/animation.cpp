#include "../../../scenelib.h"

#include "animation.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "skeleton.h"

#include "Modules/math/transformation.h"

#include "../entity.h"

#include "../entityptr.h"

namespace Engine {

namespace Scene {
    namespace Entity {

        void Animation::set(const Render::AnimationLoader::HandleType &handle)
        {
            mAnimationList = handle;
            refreshCache();
        }

        void Animation::setName(const std::string &name)
        {
            mAnimationList.load(name);
        }

        Render::AnimationLoader::ResourceType *Animation::get() const
        {
            return mAnimationList.resource();
        }

        void Animation::setCurrentAnimationName(const std::string &name)
        {
            auto it = mAnimationList->mAnimations.find(name);
            Render::AnimationDescriptor *desc = nullptr;
            if (it != mAnimationList->mAnimations.end())
                desc = &it->second;
            setCurrentAnimation(desc);
        }

        void Animation::setCurrentAnimation(Render::AnimationDescriptor *desc)
        {
            mCurrentAnimation = desc;
            mCurrentStep = 0.0f;
        }

        Render::AnimationDescriptor *Animation::currentAnimation() const
        {
            return mCurrentAnimation;
        }

        void Animation::step(float delta)
        {
            mCurrentStep += delta;
        }

        void Animation::setStep(float step)
        {
            mCurrentStep = step;
        }

        float Animation::currentStep() const
        {
            return mCurrentStep;
        }

        void Animation::refreshCache()
        {
            if (mAnimationList) {
                if (!mSkeletonCache)
                    mBoneIndexMapping = nullptr;
                else {
                    mBoneIndexMapping = mAnimationList->generateBoneMappings(mSkeletonCache.resource());
                }
            }
        }

        void Animation::applyTransform(const EntityPtr &entity)
        {
            if (mAnimationList && mCurrentAnimation) {
                Skeleton *skeleton = entity.getComponent<Skeleton>();
                if (!skeleton) {
                    mSkeletonCache.reset();
                    refreshCache();
                    return;
                }
                if (skeleton->handle() != mSkeletonCache) {
                    mSkeletonCache = entity.getComponent<Skeleton>()->handle();
                    refreshCache();
                }
                if (mSkeletonCache) {
                    float step = fmodf(mCurrentStep, mCurrentAnimation->mDuration);
                    if (step < 0.0f)
                        step += mCurrentAnimation->mDuration;
                    std::vector<Matrix4> &matrices = skeleton->matrices();
                    for (size_t i = 0; i < matrices.size(); ++i) {
                        int mappedBone = mBoneIndexMapping ? mBoneIndexMapping[i] : i;
                        if (mappedBone != -1) {
                            Render::AnimationBone &boneData = mCurrentAnimation->mBones[mappedBone];

                            auto it_position_end = std::upper_bound(boneData.mPositions.begin(), boneData.mPositions.end(), step, [](float step, const Render::KeyFrame<Vector3> &k) { return step < k.mTime; });
                            assert(it_position_end != boneData.mPositions.end());
                            assert(it_position_end != boneData.mPositions.begin());
                            auto it_position = std::prev(it_position_end);

                            auto it_scale_end = std::upper_bound(boneData.mScalings.begin(), boneData.mScalings.end(), step, [](float step, const Render::KeyFrame<Vector3> &k) { return step < k.mTime; });
                            assert(it_scale_end != boneData.mScalings.end());
                            assert(it_scale_end != boneData.mScalings.begin());
                            auto it_scale = std::prev(it_scale_end);

                            auto it_orientation_end = std::upper_bound(boneData.mOrientations.begin(), boneData.mOrientations.end(), step, [](float step, const Render::KeyFrame<Quaternion> &k) { return step < k.mTime; });
                            assert(it_orientation_end != boneData.mOrientations.end());
                            assert(it_orientation_end != boneData.mOrientations.begin());
                            auto it_orientation = std::prev(it_orientation_end);

                            float position_blend = (step - it_position->mTime) / (it_position_end->mTime - it_position->mTime);
                            float scale_blend = (step - it_scale->mTime) / (it_scale_end->mTime - it_scale->mTime);
                            float orientation_blend = (step - it_orientation->mTime) / (it_orientation_end->mTime - it_orientation->mTime);

                            Matrix4 m = TransformMatrix(
                                Lerp(it_position->mValue, it_position_end->mValue, position_blend),
                                Lerp(it_scale->mValue, it_scale_end->mValue, scale_blend),
                                Slerp(it_orientation->mValue, it_orientation_end->mValue, orientation_blend));
                            uint32_t parent = mSkeletonCache->mBones[i].mParent;
                            if (parent != std::numeric_limits<uint32_t>::max()) {
                                matrices[i] = matrices[parent] * m;
                            } else {
                                matrices[i] = mAnimationList->mBaseMatrix * m;
                            }
                        }
                    }

                    for (size_t i = 0; i < matrices.size(); ++i) {
                        matrices[i] = matrices[i] * mSkeletonCache->mBones[i].mOffsetMatrix;
                    }
                }
            }
        }

    }
}
}

ENTITYCOMPONENT_IMPL(Animation, Engine::Scene::Entity::Animation);

METATABLE_BEGIN(Engine::Scene::Entity::Animation)
PROPERTY(Animation, get, set)
PROPERTY(CurrentAnimation, currentAnimation, setCurrentAnimation)
PROPERTY(Step, currentStep, setStep)
METATABLE_END(Engine::Scene::Entity::Animation)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Animation)
SERIALIZETABLE_END(Engine::Scene::Entity::Animation)

