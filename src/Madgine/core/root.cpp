#include "../baselib.h"

#include "root.h"

#include "../resources/scripts/scriptloader.h"

#include "../resources/resourcemanager.h"
#include "Interfaces/debug/memory/memory.h"
#include "Modules/plugins/pluginmanager.h"
#include "Modules/scripting/types/luastate.h"
#include "Modules/threading/workgroup.h"

#include "Modules/cli/cli.h"
#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

namespace Engine {
namespace Core {
    Root::Root(int argc, char **argv)
        : mCLI(std::make_unique<CLI::CLI>(argc, argv))
        ,
#ifndef STATIC_BUILD
        mPluginManager(std::make_unique<Plugins::PluginManager>("Madgine"))
        , mCollectorManager(std::make_unique<UniqueComponentCollectorManager>(*mPluginManager))
        ,
#endif
        mLuaState(std::make_unique<Scripting::LuaState>())
        ,
#if ENABLE_MEMTRACKING
        mMemTracker(std::make_unique<Debug::Memory::MemoryTracker>())
        ,
#endif
        mResources(std::make_unique<Resources::ResourceManager>())
    {
#ifndef STATIC_BUILD
        (*mPluginManager)["Core"].loadPluginByFilename("Base");

        (*mPluginManager)["Utility"].loadPlugin("Tools");

        (*mPluginManager)["Core"].loadPlugin("Client");

        (*mPluginManager)["Renderer"].loadPlugin("OpenGL");
        (*mPluginManager)["Renderer"].setExclusive();
        (*mPluginManager)["Renderer"].setAtleastOne();

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
#endif

        mResources->init();

        for (auto &res : mResources->get<Scripting::Parsing::ScriptLoader>()) {
            std::shared_ptr<Scripting::Parsing::MethodHolder> p = res.second.data();
            if (p) {
                p->call(mLuaState->mainThread());
            }
        }

        mLuaState->setFinalized();
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
