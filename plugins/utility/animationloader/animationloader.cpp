#include "animationloaderlib.h"

#include "animationloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

UNIQUECOMPONENT(Engine::Render::AnimationLoader)

METATABLE_BEGIN(Engine::Render::AnimationLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::AnimationLoader)

METATABLE_BEGIN_BASE(Engine::Render::AnimationLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Render::AnimationLoader::ResourceType)

METATABLE_BEGIN(Engine::Render::AnimationList)
MEMBER(mAnimations)
METATABLE_END(Engine::Render::AnimationList)

METATABLE_BEGIN(Engine::Render::AnimationDescriptor)
MEMBER(mName)
MEMBER(mDuration)
MEMBER(mTicksPerSecond)
MEMBER(mBones)
METATABLE_END(Engine::Render::AnimationDescriptor)

METATABLE_BEGIN(Engine::Render::AnimationBone)
MEMBER(mPositions)
MEMBER(mOrientations)
MEMBER(mScalings)
METATABLE_END(Engine::Render::AnimationBone)

METATABLE_BEGIN(Engine::Render::VectorKeyFrame)
MEMBER(mTime)
MEMBER(mValue)
METATABLE_END(Engine::Render::VectorKeyFrame)

METATABLE_BEGIN(Engine::Render::QuaternionKeyFrame)
MEMBER(mTime)
MEMBER(mValue)
METATABLE_END(Engine::Render::QuaternionKeyFrame)

RegisterType(Engine::Render::AnimationLoader)

    namespace Engine
{
    namespace Render {

        AnimationLoader::AnimationLoader()
            : ResourceLoader({ ".fbx", ".dae" })
        {
        }

        bool AnimationLoader::loadImpl(AnimationList &animations, ResourceType *res)
        {
            Assimp::Importer importer;

            std::vector<unsigned char> buffer = res->readAsBlob();

            const aiScene *scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), 0);

            if (!scene) {
                LOG_ERROR(importer.GetErrorString());
                return false;
            }

            for (size_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
                aiAnimation *anim = scene->mAnimations[animationIndex];

                AnimationDescriptor &animation = animations.mAnimations.try_emplace(anim->mName.C_Str()).first->second;

                animation.mName = anim->mName.C_Str();

                animation.mDuration = anim->mDuration;
                animation.mTicksPerSecond = anim->mTicksPerSecond;

                for (size_t boneIndex = 0; boneIndex < anim->mNumChannels; ++boneIndex) {
                    aiNodeAnim *node = anim->mChannels[boneIndex];

                    AnimationBone &bone = animation.mBones.emplace_back();

                    if (animationIndex == 0) {
                        animations.mBoneNames.emplace_back(node->mNodeName.C_Str());
                    } else {
                        if (animations.mBoneNames.size() <= boneIndex) {
                            LOG_ERROR("Different amount of bones between animations in one set!");
                            animations.mBoneNames.emplace_back(node->mNodeName.C_Str());
                        } else {
                            if (animations.mBoneNames[boneIndex] != node->mNodeName.C_Str())
                                LOG_ERROR("Bone assignment mismatch between animations in one set! Will lead to broken animations!");
                        }
                    }

                    std::transform(node->mPositionKeys, node->mPositionKeys + node->mNumPositionKeys, std::back_inserter(bone.mPositions), [](const aiVectorKey &key) {
                        return KeyFrame<Vector3> { static_cast<float>(key.mTime), &key.mValue.x };
                    });

                    std::transform(node->mRotationKeys, node->mRotationKeys + node->mNumRotationKeys, std::back_inserter(bone.mOrientations), [](const aiQuatKey &key) {
                        return KeyFrame<Quaternion> { static_cast<float>(key.mTime), { key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z } };
                    });

                    std::transform(node->mScalingKeys, node->mScalingKeys + node->mNumScalingKeys, std::back_inserter(bone.mScalings), [](const aiVectorKey &key) {
                        return KeyFrame<Vector3> { static_cast<float>(key.mTime), &key.mValue.x };
                    });
                }
            }

            /*for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
                aiMesh *mesh = scene->mMeshes[meshIndex];

                for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                    aiBone *bone = mesh->mBones[boneIndex];
                    aiNode *node = scene->mRootNode->FindNode(bone->mName);

                    bool newBone = indices.try_emplace(node, skeleton.mBones.size()).second;
                    if (newBone) {

                        aiMatrix4x4 transform = node->mTransformation;

                        while (node->mParent) {
                            node = node->mParent;
                            transform = node->mTransformation * transform;
                        }

                        skeleton.mBones.push_back(
                            Bone {
                                bone->mName.C_Str(),
                                { bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
                                    bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
                                    bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
                                    bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4 },
                                { transform.a1, transform.a2, transform.a3, transform.a4,
                                    transform.b1, transform.b2, transform.b3, transform.b4,
                                    transform.c1, transform.c2, transform.c3, transform.c4,
                                    transform.d1, transform.d2, transform.d3, transform.d4 } });
                    }
                }
            }

            for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
                aiMesh *mesh = scene->mMeshes[meshIndex];

                for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                    aiBone *bone = mesh->mBones[boneIndex];
                    aiNode *node = scene->mRootNode->FindNode(bone->mName);

					size_t index = indices.at(node);

                    if (node->mParent && indices.count(node->mParent) > 0) {
                        skeleton.mBones[index].mParent = indices.at(node->mParent);
                    }

                    if (node->mNumChildren > 0 && indices.count(node->mChildren[0]) > 0) {
                        skeleton.mBones[index].mFirstChild = indices.at(node->mChildren[0]);
					}
                }
            }*/

            return true;
        }

        void AnimationLoader::unloadImpl(AnimationList &data, ResourceType *res)
        {
        }

        int *AnimationList::generateBoneMappings(const SkeletonLoader::HandleType &handle)
        {
            auto pib = mBoneMappings.try_emplace(handle);
            if (pib.second) {
                size_t i;
                bool consecutive = (mBoneNames.size() <= handle->mBones.size());
                if (!consecutive) {
                    LOG_ERROR("Skeleton/Animation bone size mismatch!");
                    pib.first->second = std::make_unique<int[]>(handle->mBones.size());
				}
                for (i = 0; i < handle->mBones.size(); ++i) {
                    if (consecutive) {
                        if (mBoneNames[i] != handle->mBones[i].mName) {
                            consecutive = false;
                            pib.first->second = std::make_unique<int[]>(handle->mBones.size());
                            for (size_t j = 0; j < i; ++j) {
                                pib.first->second[j] = j;
							}
						}
                    }
                    if (!consecutive) {
                        auto it = std::find(mBoneNames.begin(), mBoneNames.end(), handle->mBones[i].mName);
                        int mapping;
                        if (it == mBoneNames.end()) {
                            mapping = -1;
                            LOG_WARNING("Animation targets non-existing Bone '" << handle->mBones[i].mName << "'!");
                        } else {
                            mapping = it - mBoneNames.begin();
                        }
                        pib.first->second[i] = mapping;
					}
                }
            }
            return pib.first->second.get();
        }

    }
}