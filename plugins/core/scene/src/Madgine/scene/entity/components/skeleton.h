#pragma once

#include "../entitycomponent.h"

#include "skeletonloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Skeleton : EntityComponent<Skeleton> {
            Skeleton(const ObjectPtr &init);

            std::string_view getName() const;
            void setName(const std::string_view &name);

            Render::SkeletonDescriptor *data() const;

            void set(Render::SkeletonLoader::HandleType handle);

            Render::SkeletonLoader::ResourceType *get() const;
            const Render::SkeletonLoader::HandleType &handle() const;

			void resetMatrices();
            std::vector<Matrix4> &matrices();

        private:
            typename Render::SkeletonLoader::HandleType mSkeleton;

			std::vector<Matrix4> mBoneMatrices;
        };

        using SkeletonPtr = EntityComponentPtr<Skeleton>;

    }
}
}

RegisterType(Engine::Scene::Entity::Skeleton);