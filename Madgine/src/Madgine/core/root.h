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

			//std::experimental::filesystem::path mediaDir();

		private:
			const RootSettings &mSettings;

			std::unique_ptr<SignalSlot::ConnectionManager> mConnectionManger;
#ifndef STATIC_BUILD
			std::unique_ptr<Plugins::PluginManager> mPluginManager;
			std::unique_ptr<UniqueComponentCollectorManager> mCollectorManager;
#endif
			std::unique_ptr<Scripting::LuaState> mLuaState;

			std::unique_ptr<Debug::Memory::MemoryTracker> mMemTracker;
			std::unique_ptr<Debug::Profiler::Profiler> mProfiler;
			
			std::unique_ptr<Resources::ResourceManager> mResources;			
		};

	}
}