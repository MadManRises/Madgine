#include "../../../scenelib.h"

#include "skeleton.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/math/boundingbox.h"

#include "Modules/keyvalue/valuetype.h"

#include "skeletondescriptor.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        Skeleton::Skeleton(const ObjectPtr &data)
            : EntityComponent(data)
        {
            if (const Engine::ValueType &v = data["skeleton"]; v.is<std::string>()) {
                setName(v.as<std::string>());
            }
        }
        Render::SkeletonDescriptor *Skeleton::data() const
        {
            return mSkeleton;
        }

        std::string Skeleton::getName() const
        {
            return mSkeleton.name();
        }

        void Skeleton::setName(const std::string &name)
        {
            mSkeleton.load(name);
            resetMatrices();
        }

        void Skeleton::set(Render::SkeletonLoader::HandleType handle)
        {
            mSkeleton = handle;
            resetMatrices();
        }

        Render::SkeletonLoader::ResourceType *Skeleton::get() const
        {
            return mSkeleton.resource();
        }

        const Render::SkeletonLoader::HandleType &Skeleton::handle() const
        {
            return mSkeleton;
        }

        void Skeleton::resetMatrices()
        {
            if (!mSkeleton)
                mBoneMatrices.clear();
            else {
                mBoneMatrices.resize(mSkeleton->mBones.size());

				for (size_t i = 0; i < mBoneMatrices.size(); ++i) {
                    mBoneMatrices[i] = mSkeleton->mBones[i].mTTransform/* * mSkeleton->mBones[i].mOffsetMatrix*/;
				}
			}
        }

        std::vector<Matrix4> &Skeleton::matrices()
        {
            return mBoneMatrices;
        }
    }
}
}

ENTITYCOMPONENT_IMPL(Skeleton, Engine::Scene::Entity::Skeleton);

METATABLE_BEGIN(Engine::Scene::Entity::Skeleton)
PROPERTY(Skeleton, get, set)
READONLY_PROPERTY(Matrices, matrices)
METATABLE_END(Engine::Scene::Entity::Skeleton)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Skeleton)
ENCAPSULATED_FIELD(Skeleton, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Skeleton)


