#pragma once


namespace Engine
{
	namespace Core
	{

		struct RootSettings
		{
			std::experimental::filesystem::path mMediaDir;
		};
		
		class MADGINE_BASE_EXPORT Root
		{
		public:
			Root(const RootSettings &settings);
			~Root();

			bool init();

			Plugins::PluginManager &pluginMgr();
			Scripting::LuaState &luaState();
			Resources::ResourceManager &resources();

			std::experimental::filesystem::path mediaDir();

		private:
			const RootSettings &mSettings;

			std::unique_ptr<SignalSlot::ConnectionManager> mConnectionManger;
			std::unique_ptr<Plugins::PluginManager> mPluginManager;
			std::unique_ptr<Scripting::LuaState> mLuaState;
			
			std::unique_ptr<Resources::ResourceManager> mResources;			
		};

	}
}