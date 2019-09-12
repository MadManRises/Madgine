#include "../baselib.h"

#include "root.h"

#include "Modules/resources/resourcemanager.h"
#include "Interfaces/debug/memory/memory.h"
#include "Modules/plugins/pluginmanager.h"
#include "Modules/threading/workgroup.h"

#include "Modules/cli/cli.h"
#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

namespace Engine {
namespace Core {
    Root::Root(int argc, char **argv)
        : mCLI(std::make_unique<CLI::CLICore>(argc, argv))
        ,
#if ENABLE_PLUGINS
        mPluginManager(std::make_unique<Plugins::PluginManager>())
        , mCollectorManager(std::make_unique<UniqueComponentCollectorManager>(*mPluginManager))
        ,
#endif
#if ENABLE_MEMTRACKING
        mMemTracker(std::make_unique<Debug::Memory::MemoryTracker>())
        ,
#endif
			mResources(std::make_unique<Resources::ResourceManager>())
		{
#if ENABLE_PLUGINS
                    mPluginManager->loadCurrentSelectionFile();

        (*mPluginManager)["Core"].loadPlugin("Client");

        (*mPluginManager)["Renderer"].loadPlugin("OpenGL");
        (*mPluginManager)["Renderer"].setExclusive();
        (*mPluginManager)["Renderer"].setAtleastOne();

		(*mPluginManager)["Tools"].loadPlugin("OpenGLTools");

        (*mPluginManager)["Input"].setExclusive();
        (*mPluginManager)["Input"].setAtleastOne();

        (*mPluginManager)["Core"].loadPlugin("Common");
        (*mPluginManager)["Core"].loadPlugin("Static");
        (*mPluginManager)["Core"].loadPlugin("Camera");
        (*mPluginManager)["Core"].loadPlugin("TWClient");
        (*mPluginManager)["Core"].loadPlugin("Menues");
        (*mPluginManager)["Core"].loadPlugin("Animation");
        (*mPluginManager)["Core"].loadPlugin("IngameUI");
        (*mPluginManager)["Core"].loadPlugin("TWServer");

        (*mPluginManager)["Test"];

		mPluginManager->executeCLI();
#endif

        mResources->init();
    }

    Root::~Root()
    {
    }

    int Root::errorCode()
    {
        return mErrorCode;
    }
}
}
