#include "../baselib.h"

#include "root.h"

#include "Interfaces/debug/memory/memory.h"
#include "Modules/keyvalueutil/keyvalueregistry.h"
#include "Modules/plugins/pluginmanager.h"
#include "Modules/resources/resourcemanager.h"
#include "Modules/threading/workgroup.h"

#include "Modules/cli/cli.h"
#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "Modules/plugins/pluginsection.h"

#include "Modules/threading/taskguard.h"

namespace Engine {
namespace Core {

    Root::Root(const std::string &programName, int argc, char **argv)
        : mCLI(std::make_unique<CLI::CLICore>(argc, argv))
#if ENABLE_PLUGINS
        , mPluginManager(std::make_unique<Plugins::PluginManager>(programName))
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

        mResources->init();
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
