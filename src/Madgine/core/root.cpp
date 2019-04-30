#include "../baselib.h"

#include "root.h"

#include "../resources/scripts/scriptloader.h"

#include "Interfaces/scripting/types/luastate.h"
#include "Interfaces/plugins/pluginmanager.h"
#include "../resources/resourcemanager.h"
#include "Interfaces/threading/workgroup.h"
#include "Interfaces/debug/memory/memory.h"


#include "Interfaces/uniquecomponent/uniquecomponentcollectormanager.h"


namespace Engine
{
	namespace Core
	{
		Root::Root() :
#ifndef STATIC_BUILD
		mPluginManager(std::make_unique<Plugins::PluginManager>("Madgine")),
		mCollectorManager(std::make_unique<UniqueComponentCollectorManager>(*mPluginManager)),
#endif		
		mLuaState(std::make_unique<Scripting::LuaState>()),
#ifdef ENABLE_MEMTRACKING
			mMemTracker(std::make_unique<Debug::Memory::MemoryTracker>()),
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

			mPluginManager->section("TW", "Core").loadPlugin("Common");
			mPluginManager->section("TW", "Core").loadPlugin("Static");
			mPluginManager->section("TW", "Core").loadPlugin("Camera");
			mPluginManager->section("TW", "Core").loadPlugin("TWClient");
			mPluginManager->section("TW", "Core").loadPlugin("Menues");
			mPluginManager->section("TW", "Core").loadPlugin("Animation");
			mPluginManager->section("TW", "Core").loadPlugin("TWServer");
#endif

			mResources->init();

			for (auto &res : mResources->get<Scripting::Parsing::ScriptLoader>())
			{
				std::shared_ptr<Scripting::Parsing::MethodHolder> p = res.second.data();
				if (p)
				{
					p->call(mLuaState->mainThread());
				}
			}

			mLuaState->setFinalized();

		}

		Root::~Root()
		{
		}

	}
}
