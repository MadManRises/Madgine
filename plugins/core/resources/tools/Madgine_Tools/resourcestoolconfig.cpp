#include "resourcestoolslib.h"

#include "resourcestoolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "Madgine_Tools/renderer/imroot.h"

#include "Madgine/resources/resourceloaderbase.h"
#include "Madgine/resources/resourcemanager.h"

namespace Engine {
namespace Tools {

    ResourcesToolConfig::ResourcesToolConfig(ImRoot &root)
        : Tool<ResourcesToolConfig>(root)
    {
    }

    Threading::Task<bool> ResourcesToolConfig::init()
    {
        for (std::unique_ptr<Resources::ResourceLoaderBase> &loader : Resources::ResourceManager::getSingleton().mCollector) {
            for (const MetaTable *type : loader->resourceTypes()) {
                mRoot.getTool<Inspector>().addObjectSuggestion(type, [&]() {
                    return loader->typedResources();
                });
            }
        }

        co_return true;
    }

    void ResourcesToolConfig::renderMenu()
    {
    }

    std::string_view ResourcesToolConfig::key() const
    {
        return "ResourcesToolConfig";
    }

}
}

UNIQUECOMPONENT(Engine::Tools::ResourcesToolConfig);

METATABLE_BEGIN_BASE(Engine::Tools::ResourcesToolConfig, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::ResourcesToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ResourcesToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::ResourcesToolConfig)
