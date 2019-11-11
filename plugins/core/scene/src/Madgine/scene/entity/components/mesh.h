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

            void setManual(std::shared_ptr<Resources::MeshData> data);

            void set(Resources::MeshLoader::ResourceType *mesh);
            Resources::MeshLoader::ResourceType *get() const;

            void setVisible(bool vis);
            bool isVisible() const;

        private:
            std::shared_ptr<Resources::MeshData> mData;
            typename Resources::MeshLoader::ResourceType *mResource;
        };

    }
}
}