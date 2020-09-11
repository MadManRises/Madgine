#pragma once

#include "../entitycomponent.h"

#include "gpumeshloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Mesh : EntityComponent<Mesh> {
            Mesh(const ObjectPtr &init);

            std::string getName() const;
            void setName(const std::string &name);

            Render::GPUMeshData *data() const;

            AABB aabb() const;

            void set(Render::GPUMeshLoader::HandleType handle);            

            Render::GPUMeshLoader::ResourceType *get() const;
            const Render::GPUMeshLoader::HandleType &handle() const;

            void setVisible(bool vis);
            bool isVisible() const;

        private:
            typename Render::GPUMeshLoader::HandleType mMesh;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::Mesh);