#include "../baselib.h"

#include "root.h"

#include "../scripting/parsing/scriptloader.h"

#include <windows.h>

namespace Engine
{
	namespace Core
	{
		Root::Root(const RootSettings &settings) :
			mSettings(settings),
		mPluginManager("Madgine")
		{
			LoadLibrary("Tools_d.dll");
		}

		bool Root::init()
		{
			mResources = std::make_unique<Resources::ResourceManager>(*this, mSettings.mMediaDir);
			if (!mResources->init())
				return false;

			for(auto &res : mResources->get<Scripting::Parsing::ScriptLoader>())
			{
				std::shared_ptr<Scripting::Parsing::MethodHolder> p = res.second.data();
				if (p)
				{
					p->call(mLuaState.state());
				}
			}

			mLuaState.setFinalized();

			return true;

		}

		Plugins::PluginManager& Root::pluginMgr()
		{
			return mPluginManager;
		}

		Scripting::LuaState& Root::luaState()
		{
			return mLuaState;
		}

		Resources::ResourceManager& Root::resources()
		{
			return *mResources;
		}

		std::experimental::filesystem::path Root::mediaDir()
		{
			return mSettings.mMediaDir;
		}


	}
}
