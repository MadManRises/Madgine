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

            Render::MeshData *data() const;

            AABB aabb() const;

            void set(Render::MeshLoader::HandleType handle);            

            Render::MeshLoader::ResourceType *get() const;
            const Render::MeshLoader::HandleType &handle() const;

            void setVisible(bool vis);
            bool isVisible() const;

        private:
            typename Render::MeshLoader::HandleType mMesh;
        };

    }
}
}