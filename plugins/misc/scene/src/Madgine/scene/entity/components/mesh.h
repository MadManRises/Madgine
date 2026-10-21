#pragma once

#include "Madgine/meshloader/gpumeshloader.h"

#include "../entitycomponent.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Mesh : EntityComponent<Mesh> {

            const Render::GPUMeshData *data() const;

            Math::AABB aabb() const;

            void setVisible(bool visible);
            void setInheritVisibility(bool inherit);

            bool visible() const;
            bool worldVisible(Contextual<Entity &> entity) const;
            bool inheritsVisibility() const;

            typename Render::GPUMeshLoader::Handle mMesh;

            uint32_t mMaterial = 0;

            bool mIsVisible = true;
            bool mInheritsVisibility = false;
        };

    }
}
}