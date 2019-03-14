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
			(*mPluginManager)["Utility"].loadPlugin("Tools");

			(*mPluginManager)["Core"].loadPlugin("Client");

			(*mPluginManager)["Renderer"].loadPlugin("OpenGL");
			(*mPluginManager)["Renderer"].setExclusive();
			(*mPluginManager)["Renderer"].setAtleastOne();

			(*mPluginManager)["Input"].setExclusive();
			(*mPluginManager)["Input"].setAtleastOne();
#endif

			mResources->init();

			for (auto &res : mResources->get<Scripting::Parsing::ScriptLoader>())
			{
				std::shared_ptr<Scripting::Parsing::MethodHolder> p = res.second.data();
				if (p)
				{
					p->call(mLuaState->state());
				}
			}

			mLuaState->setFinalized();

		}

		Root::~Root()
		{
		}

	}
}
