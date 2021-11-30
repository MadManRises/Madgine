#include "../../../scenelib.h"

#include "skeleton.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/valuetype.h"

#include "skeletondescriptor.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        Skeleton::Skeleton(const ObjectPtr &data)
            : NamedUniqueComponent(data)
        {
            Engine::ValueType v;
            if (data.getValue(v, "skeleton") && v.is<std::string>()) {
                setName(v.as<std::string>());
            }
        }
        const Render::SkeletonDescriptor *Skeleton::data() const
        {
            return mSkeleton;
        }

        std::string_view Skeleton::getName() const
        {
            return mSkeleton.name();
        }

        void Skeleton::setName(std::string_view name)
        {
            mSkeleton.load(name);
            resetMatrices();
        }

        void Skeleton::set(Render::SkeletonLoader::HandleType handle)
        {
            mSkeleton = std::move(handle);
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
                    mBoneMatrices[i] = mSkeleton->mBones[i].mTTransform;
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


