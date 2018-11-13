#include "../baselib.h"

#include "root.h"

#include "../resources/scripts/scriptloader.h"

#include "Interfaces/scripting/types/luastate.h"
#include "Interfaces/plugins/pluginmanager.h"
#include "../resources/resourcemanager.h"
#include "Interfaces/signalslot/connectionmanager.h"
#include "Interfaces/debug/memory/memory.h"
#include "Interfaces/debug/profiler/profiler.h"

#include "../uniquecomponentcollectormanager.h"


namespace Engine
{
	namespace Core
	{
		Root::Root(const RootSettings &settings) :
			mSettings(settings),
#ifndef STATIC_BUILD
		mPluginManager(std::make_unique<Plugins::PluginManager>("Madgine")),
			mCollectorManager(std::make_unique<UniqueComponentCollectorManager>(*mPluginManager)),
#endif
		mConnectionManger(std::make_unique<SignalSlot::ConnectionManager>()),
		mLuaState(std::make_unique<Scripting::LuaState>()),
			mMemTracker(std::make_unique<Debug::Memory::MemoryTracker>()),
			mProfiler(std::make_unique<Debug::Profiler::Profiler>())
		{
#ifndef STATIC_BUILD
			(*mPluginManager)["Utility"].loadPlugin("Tools");
#endif
		}

		Root::~Root()
		{
		}

		bool Root::init()
		{
			mResources = std::make_unique<Resources::ResourceManager>(mSettings.mMediaDir);
			if (!mResources->init())
				return false;

			for(auto &res : mResources->get<Scripting::Parsing::ScriptLoader>())
			{
				std::shared_ptr<Scripting::Parsing::MethodHolder> p = res.second.data();
				if (p)
				{
					p->call(mLuaState->state());
				}
			}

			mLuaState->setFinalized();

#ifndef STATIC_BUILD
			(*mPluginManager)["Renderer"].loadPlugin("OpenGL");
			(*mPluginManager)["Renderer"].setExclusive();
			(*mPluginManager)["Renderer"].setAtleastOne();
#endif

			return true;

		}

/*		std::experimental::filesystem::path Root::mediaDir()
		{
			return mSettings.mMediaDir;
		}*/


	}
}
