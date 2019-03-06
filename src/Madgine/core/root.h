#pragma once


namespace Engine
{
	namespace Core
	{

		class MADGINE_BASE_EXPORT Root
		{
		public:
			Root();
			~Root();

			bool init();

		private:

#ifndef STATIC_BUILD
			std::unique_ptr<Plugins::PluginManager> mPluginManager;
			std::unique_ptr<UniqueComponentCollectorManager> mCollectorManager;
#endif
			std::unique_ptr<Scripting::LuaState> mLuaState;

#ifdef ENABLE_MEMTRACKING
			std::unique_ptr<Debug::Memory::MemoryTracker> mMemTracker;
#endif
			
			std::unique_ptr<Resources::ResourceManager> mResources;			
		};

	}
}