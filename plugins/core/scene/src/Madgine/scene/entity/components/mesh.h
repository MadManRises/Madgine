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

            void setManual(Render::MeshLoader::HandleType handle);
            void set(Render::MeshLoader::ResourceType *res);

            Render::MeshLoader::ResourceType *get() const;

            void setVisible(bool vis);
            bool isVisible() const;

        private:
            typename Render::MeshLoader::HandleType mMesh;
        };

    }
}
}