#pragma once

#include "../scripting/types/luastate.h"
#include "../plugins/pluginmanager.h"
#include "../resources/resourcemanager.h"
#include "../signalslot/connectionmanager.h"

namespace Engine
{
	namespace App
	{

		struct RootSettings
		{
			std::experimental::filesystem::path mMediaDir;
		};
		
		class MADGINE_BASE_EXPORT Root
		{
		public:
			Root(const RootSettings &settings);

			bool init();

			Plugins::PluginManager &pluginMgr();
			Scripting::LuaState &luaState();
			Resources::ResourceManager &resources();

			std::experimental::filesystem::path mediaDir();

		private:
			const RootSettings &mSettings;

			SignalSlot::ConnectionManager mConnectionManger;
			Plugins::PluginManager mPluginManager;
			Scripting::LuaState mLuaState;
			
			std::unique_ptr<Resources::ResourceManager> mResources;			
		};

	}
}