#pragma once

#include "Modules/generic/container/generationvector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityPtr {

            EntityPtr() = default;

            EntityPtr(const typename GenerationVector<Entity>::iterator it);

            Entity *operator->() const;

            operator bool() const;

            operator Entity *() const;

            bool operator==(const EntityPtr &other) const;

        private:
            typename GenerationVector<Entity>::iterator mIt;
        };

    }
}
}