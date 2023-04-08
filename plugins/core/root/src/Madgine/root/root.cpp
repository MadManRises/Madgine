#include "../rootlib.h"

#include "root.h"

#include "Interfaces/debug/memory/memory.h"
#include "Modules/plugins/pluginmanager.h"
#include "keyvalueregistry.h"

#include "Madgine/cli/cli.h"
#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "Madgine/cli/parameter.h"

#include "rootcomponentbase.h"

#include "Interfaces/util/standardlog.h"

#include "Interfaces/filesystem/async.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

namespace Engine {
namespace Root {

    static Root *sSingleton = nullptr;

#if ENABLE_PLUGINS
    CLI::Parameter<bool> noPluginCache { { "--no-plugin-cache", "-npc" }, false, "Disables the loading of the cached plugin selection at startup." };
    CLI::Parameter<std::string> loadPlugins { { "--load-plugins", "-lp" }, "", "If set the pluginmanager will load the specified config file after loading the cached plugin-file." };
#endif

    CLI::Parameter<bool> toolModeParameter { { "--toolMode", "-t" }, false, "If set, no application will be started. Only the root will be initialized and then immediately shutdown again." };
    CLI::Parameter<Engine::Util::MessageType> logLevel { { "--logLevel", "-l" }, Engine::Util::MessageType::INFO_TYPE, "Specify log-level." };

    Root::Root(int argc, char **argv)
        : Root(std::make_unique<CLI::CLICore>(argc, argv))
    {
    }

    Root::Root(std::unique_ptr<CLI::CLICore> cli)
        : mCLI(std::move(cli))
        , mTaskQueue("Root")
    {

        assert(!sSingleton);
        sSingleton = this;

        Util::StandardLog::setLogLevel(logLevel);

#if ENABLE_PLUGINS
        mPluginManager = std::make_unique<Plugins::PluginManager>();
        mErrorCode = mPluginManager->setup(!noPluginCache, mCLI->mProgramPath.stem(), loadPlugins);
        mCollectorManager = std::make_unique<UniqueComponent::CollectorManager>(*mPluginManager);
#endif

#if ENABLE_MEMTRACKING
        mMemTracker = std::make_unique<Debug::Memory::MemoryTracker>();
#endif

        mComponents = std::make_unique<RootComponentContainer<std::vector<Placeholder<0>>>>(*this);

        for (std::unique_ptr<RootComponentBase> &component : *mComponents) {
            KeyValueRegistry::registerGlobal(component->key().data(), component.get());

            if (mErrorCode == 0)
                mErrorCode = component->mErrorCode;
        }

        mTaskQueue.queue([this]() -> Threading::Task<void> {
            while (mTaskQueue.running()) {
                Filesystem::checkAsyncIOCompletion();
                co_await 500ms;
            }
            do {
                Filesystem::cancelAllAsyncIO();
                Filesystem::checkAsyncIOCompletion();
                co_await 0ms;
            } while (Filesystem::pendingIOOperationCount() > 0);
        });
    }

    Root::~Root()
    {
        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    Root &Root::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    RootComponentBase &Root::getComponent(size_t i)
    {
        return mComponents->get(i);
    }

    bool Root::toolMode() const
    {
        return toolModeParameter;
    }

    int Root::errorCode()
    {
        return mErrorCode;
    }

    Threading::TaskQueue *Root::taskQueue()
    {
        return &mTaskQueue;
    }

}
}
