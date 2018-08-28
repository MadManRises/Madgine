#include "../baselib.h"

#include "root.h"

#include "../scripting/parsing/scriptloader.h"

#include "../scripting/types/luastate.h"
#include "../plugins/pluginmanager.h"
#include "../resources/resourcemanager.h"
#include "../signalslot/connectionmanager.h"


namespace Engine
{
	namespace Core
	{
		Root::Root(const RootSettings &settings) :
			mSettings(settings),
		mPluginManager(std::make_unique<Plugins::PluginManager>("Madgine")),
		mConnectionManger(std::make_unique<SignalSlot::ConnectionManager>()),
		mLuaState(std::make_unique<Scripting::LuaState>())
		{
			(*mPluginManager)["Utility"].getPlugin("Tools").load();
		}

		Root::~Root()
		{
		}

		bool Root::init()
		{
			mResources = std::make_unique<Resources::ResourceManager>(*mPluginManager, mSettings.mMediaDir);
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

			return true;

		}

		std::experimental::filesystem::path Root::mediaDir()
		{
			return mSettings.mMediaDir;
		}


	}
}
