#pragma once

#include "Modules/plugins/pluginlocal.h"

#include "Modules/keyvalue/objectptr.h"

#include "Modules/keyvalue/metatable.h"

#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "Modules/uniquecomponent/indexref.h"

DECLARE_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponent, EntityComponentBase, Entity &, const ObjectPtr &);


namespace Engine {
namespace Scene {
    namespace Entity {

        MADGINE_SCENE_EXPORT std::map<std::string_view, IndexRef> &sComponentsByName();

    }
}
}