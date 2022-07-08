#include "../../../scenelib.h"

#include "animation.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "skeleton.h"

#include "Meta/math/transformation.h"

#include "../entity.h"

#include "../entitycomponentptr.h"

ENTITYCOMPONENT_IMPL(Animation, Engine::Scene::Entity::Animation);

METATABLE_BEGIN(Engine::Scene::Entity::Animation)
PROPERTY(Animation, get, set)
PROPERTY(CurrentAnimation, currentAnimationPtr, setCurrentAnimationPtr)
PROPERTY(Step, currentStep, setStep)
METATABLE_END(Engine::Scene::Entity::Animation)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Animation)
ENCAPSULATED_FIELD(Animation, getName, setName)
FIELD(mCurrentAnimation)
SERIALIZETABLE_END(Engine::Scene::Entity::Animation)

namespace Engine {

namespace Scene {
    namespace Entity {

        void Animation::set(Render::AnimationLoader::HandleType handle)
        {
            mAnimationList = std::move(handle);
            setCurrentAnimation({});
            refreshCache();
        }
        
        std::string_view Animation::getName() const
        {
            return mAnimationList.name();
        }

        void Animation::setName(std::string_view name)
        {
            mAnimationList.load(name);
            setCurrentAnimation({});
            refreshCache();
        }

        Render::AnimationLoader::ResourceType *Animation::get() const
        {
            return mAnimationList.resource();
        }

        void Animation::setCurrentAnimationName(std::string_view name)
        {
            auto it = std::ranges::find(mAnimationList->mAnimations, name, &Render::AnimationDescriptor::mName);
            IndexType<uint32_t> index;
            if (it != mAnimationList->mAnimations.end())
                index = std::distance(mAnimationList->mAnimations.begin(), it);
            setCurrentAnimation(index);
        }

        void Animation::setCurrentAnimationPtr(Render::AnimationDescriptor *desc)
        {
            auto it = std::ranges::find_if(mAnimationList->mAnimations, [=](const Render::AnimationDescriptor &d) { return &d == desc; });
            IndexType<uint32_t> index;
            if (it != mAnimationList->mAnimations.end())
                index = std::distance(mAnimationList->mAnimations.begin(), it);
            setCurrentAnimation(index);
        }

        IndexType<uint32_t> Animation::currentAnimation() const
        {
            return mCurrentAnimation;
        }

        void Animation::setCurrentAnimation(IndexType<uint32_t> index)
        {
            mCurrentAnimation = index;
            mCurrentStep = 0.0f;
        }

        Render::AnimationDescriptor *Animation::currentAnimationPtr() const
        {
            return mCurrentAnimation ? &mAnimationList->mAnimations[mCurrentAnimation] : nullptr;
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
            if (mAnimationList.available()) {
                if (!mSkeletonCache.available())
                    mBoneIndexMapping = nullptr;
                else {
                    mBoneIndexMapping = mAnimationList->generateBoneMappings(mSkeletonCache.resource());
                }
            }
        }

        void Animation::updateRender(Entity *entity)
        {
            if (mAnimationList.available() && mCurrentAnimation) {
                Render::AnimationDescriptor &animation = mAnimationList->mAnimations[mCurrentAnimation];
                Skeleton *skeleton = entity->getComponent<Skeleton>();
                if (!skeleton) {
                    mSkeletonCache.reset();
                    refreshCache();
                    return;
                }
                if (skeleton->handle() != mSkeletonCache) {
                    mSkeletonCache = entity->getComponent<Skeleton>()->handle();
                    refreshCache();
                }
                if (mSkeletonCache) {
                    float step = fmodf(mCurrentStep, animation.mDuration);
                    if (step < 0.0f)
                        step += animation.mDuration;

                    std::vector<Matrix4> &matrices = skeleton->matrices();
                    std::set<size_t> parentTransformToDos;

                    for (size_t i = 0; i < matrices.size(); ++i) {
                        int mappedBone = mBoneIndexMapping ? mBoneIndexMapping[i] : i;
                        if (mappedBone != -1) {
                            Render::AnimationBone &boneData = animation.mBones[mappedBone];

                            auto it_position_end = std::upper_bound(boneData.mPositions.begin(), boneData.mPositions.end(), step, [](float step, const Render::KeyFrame<Vector3> &k) { return step < k.mTime; });
                            assert(it_position_end != boneData.mPositions.begin());
                            auto it_position = std::prev(it_position_end);

                            Vector3 pos;
                            if (it_position_end != boneData.mPositions.end()) {
                                float position_blend = (step - it_position->mTime) / (it_position_end->mTime - it_position->mTime);
                                pos = lerp(it_position->mValue, it_position_end->mValue, position_blend);
                            } else {
                                pos = it_position->mValue;
                            }

                            auto it_scale_end = std::upper_bound(boneData.mScalings.begin(), boneData.mScalings.end(), step, [](float step, const Render::KeyFrame<Vector3> &k) { return step < k.mTime; });
                            assert(it_scale_end != boneData.mScalings.begin());
                            auto it_scale = std::prev(it_scale_end);

                            Vector3 scale;
                            if (it_scale_end != boneData.mScalings.end()) {
                                float scale_blend = (step - it_scale->mTime) / (it_scale_end->mTime - it_scale->mTime);
                                scale = lerp(it_scale->mValue, it_scale_end->mValue, scale_blend);
                            } else {
                                scale = it_scale->mValue;
                            }

                            auto it_orientation_end = std::upper_bound(boneData.mOrientations.begin(), boneData.mOrientations.end(), step, [](float step, const Render::KeyFrame<Quaternion> &k) { return step < k.mTime; });
                            assert(it_orientation_end != boneData.mOrientations.begin());
                            auto it_orientation = std::prev(it_orientation_end);

                            Quaternion orientation;
                            if (it_orientation_end != boneData.mOrientations.end()) {
                                float orientation_blend = (step - it_orientation->mTime) / (it_orientation_end->mTime - it_orientation->mTime);
                                orientation = slerp(it_orientation->mValue, it_orientation_end->mValue, orientation_blend);
                            } else {
                                orientation = it_orientation->mValue;
                            }

                            matrices[i] = mSkeletonCache->mBones[i].mPreTransform * TransformMatrix(
                                pos,
                                scale,
                                orientation);

                            IndexType<uint32_t> parent = mSkeletonCache->mBones[i].mParent;
                            if (parent) {
                                /*if (parent < i && parentTransformToDos.count(parent) == 0)
                                    matrices[i] = matrices[parent] * matrices[i];
                                else*/
                                parentTransformToDos.emplace(i);
                            }
                        }
                    }
                    while (!parentTransformToDos.empty()) {
                        for (std::set<size_t>::iterator it = parentTransformToDos.begin(); it != parentTransformToDos.end();) {
                            size_t parentIndex = mSkeletonCache->mBones[*it].mParent;
                            if (parentTransformToDos.count(parentIndex) == 0) {
                                matrices[*it] = matrices[parentIndex] * matrices[*it];
                                it = parentTransformToDos.erase(it);
                            } else {
                                ++it;
                            }
                        }
                    }

                    for (size_t i = 0; i < matrices.size(); ++i) {
                        int mappedBone = mBoneIndexMapping ? mBoneIndexMapping[i] : i;
                        if (mappedBone != -1)
                            matrices[i] = mSkeletonCache->mMatrix * matrices[i] * mSkeletonCache->mBones[i].mOffsetMatrix;
                    }
                }
            }
        }

    }
}
}


