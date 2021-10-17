#include "../baselib.h"

#include "root.h"

#include "Interfaces/debug/memory/memory.h"
#include "Madgine/resources/resourcemanager.h"
#include "Modules/plugins/pluginmanager.h"
#include "Modules/threading/workgroup.h"
#include "keyvalueregistry.h"

#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"
#include "cli/cli.h"

#include "Modules/plugins/pluginsection.h"

namespace Engine {
namespace Core {

    Root::Root(int argc, char **argv)
        : Root(std::make_unique<CLI::CLICore>(argc, argv))
    {
    }

    Root::Root(std::unique_ptr<CLI::CLICore> cli)
        : mCLI(std::move(cli))
#if ENABLE_PLUGINS
        , mPluginManager(std::make_unique<Plugins::PluginManager>())
        , mCollectorManager(std::make_unique<UniqueComponentCollectorManager>(*mPluginManager))
#endif
#if ENABLE_MEMTRACKING
        , mMemTracker(std::make_unique<Debug::Memory::MemoryTracker>())
#endif
        , mResources(std::make_unique<Resources::ResourceManager>())
    {
#if ENABLE_PLUGINS
        mPluginManager->setup();
#endif

        KeyValueRegistry::registerGlobal("ResourceManager", mResources.get());
    }

    Root::~Root()
    {
        KeyValueRegistry::unregisterGlobal(mResources.get());
    }

    int Root::errorCode()
    {
        return mErrorCode;
    }
}
}
