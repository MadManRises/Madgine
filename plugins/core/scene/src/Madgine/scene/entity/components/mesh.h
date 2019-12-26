#pragma once

#include "../entitycomponent.h"

#include "meshloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Mesh : EntityComponent<Mesh> {
            Mesh(Entity &e, const ObjectPtr &init);

            std::string getName() const;
            void setName(const std::string &name);

            Resources::MeshData *data() const;

            AABB aabb() const;

            void setManual(Resources::MeshLoader::HandleType handle);
            void set(Resources::MeshLoader::ResourceType *res);

            Resources::MeshLoader::ResourceType *get() const;

            void setVisible(bool vis);
            bool isVisible() const;

        private:
            typename Resources::MeshLoader::HandleType mMesh;
        };

    }
}
}