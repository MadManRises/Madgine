#include "../../../scenelib.h"

#include "skeleton.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/valuetype.h"

#include "skeletondescriptor.h"

#include "Madgine/resources/resourcemanager.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        Skeleton::Skeleton(const ObjectPtr &data)
            : NamedComponent(data)
        {
            Engine::ValueType v;
            if (data.getValue(v, "skeleton") && v.is<std::string>()) {
                setName(v.as<std::string>());
            }
        }
        const Render::SkeletonDescriptor *Skeleton::data() const
        {
            if (mLoaded)
                return mSkeleton;
            else
                return nullptr;
        }

        std::string_view Skeleton::getName() const
        {
            return mSkeleton.name();
        }

        void Skeleton::setName(std::string_view name)
        {
            mLoaded = false;
            mSkeleton.load(name);
            mSkeleton.info()->loadingTask().then([this](bool) { resetMatrices(); mLoaded = true; }, Resources::ResourceManager::getSingleton().taskQueue());
        }

        void Skeleton::set(Render::SkeletonLoader::Handle handle)
        {
            mLoaded = false;
            mSkeleton = std::move(handle);
            mSkeleton.info()->loadingTask().then([this](bool) { resetMatrices(); mLoaded=true; }, Resources::ResourceManager::getSingleton().taskQueue());
        }

        Render::SkeletonLoader::Resource *Skeleton::get() const
        {
            return mSkeleton.resource();
        }

        const Render::SkeletonLoader::Handle &Skeleton::handle() const
        {
            return mSkeleton;
        }

        void Skeleton::resetMatrices()
        {
            if (!mSkeleton.available())
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
