#pragma once

#include "Modules/keyvalueutil/virtualscopebase.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableUnitBase, VirtualScopeBase {        
            EntityComponentBase(const ObjectPtr &initTable = {});
            virtual ~EntityComponentBase() = 0;

            virtual void init(const EntityPtr &entity);
            virtual void finalize(const EntityPtr &entity);

            virtual const std::string_view &key() const = 0;

        protected:
            const ObjectPtr &initTable();

        private:
            ObjectPtr mInitTable;
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);