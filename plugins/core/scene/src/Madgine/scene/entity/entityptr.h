#pragma once

#include "Modules/generic/container/generationvector.h"
#include "Modules/keyvalue/objectptr.h"
#include "Modules/generic/future.h"
#include "entityhandle.h"
#include "Modules/keyvalue/scopebase.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityPtr : ScopeBase {

            EntityPtr() = default;
            ~EntityPtr();

            EntityPtr(const EntityPtr &other);
            EntityPtr(ControlBlock<Entity> &entity);
            

            EntityPtr &operator=(const EntityPtr &other);
            EntityPtr &operator=(EntityPtr &&other);

            Entity *operator->() const;

            void update() const;
            //operator bool() const;

            operator Entity *() const;
            Entity *get() const;

            bool operator==(const EntityPtr &other) const;            

            TypedScopePtr customScopePtr() const;

            bool isDead() const;

        private:            
            mutable ControlBlock<Entity> *mEntity = nullptr;
        };

    }
}
}