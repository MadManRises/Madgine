#include "../../scenelib.h"

#include "entitycomponentcollector.h"

#include "Modules/plugins/pluginmanager.h"

#include "componentexception.h"

#include "Modules/keyvalue/keyvalue.h"

#include "entitycomponentbase.h"

#include "Modules/plugins/pluginsection.h"

#include "Modules/plugins/plugin.h"

DEFINE_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponent)

namespace Engine {
namespace Scene {
    namespace Entity {

        std::map<std::string_view, IndexRef> &sComponentsByName()
        {
            static std::map<std::string_view, IndexRef> dummy;
            return dummy;
        }

    }
}
}