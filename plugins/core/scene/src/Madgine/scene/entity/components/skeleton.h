#pragma once

#include "../entitycomponent.h"

#include "skeletonloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Skeleton : EntityComponent<Skeleton> {
            Skeleton(const ObjectPtr &init);

            std::string_view getName() const;
            void setName(std::string_view name);

            const Render::SkeletonDescriptor *data() const;

            void set(Render::SkeletonLoader::Handle handle);

            Render::SkeletonLoader::Resource *get() const;
            const Render::SkeletonLoader::Handle &handle() const;

			void resetMatrices();
            std::vector<Matrix4> &matrices();

        private:
            typename Render::SkeletonLoader::Handle mSkeleton;

			std::vector<Matrix4> mBoneMatrices;

            bool mLoaded = true;
        };

        using SkeletonPtr = EntityComponentPtr<Skeleton>;

    }
}
}

REGISTER_TYPE(Engine::Scene::Entity::Skeleton)
REGISTER_TYPE(Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>)