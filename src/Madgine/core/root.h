#pragma once


namespace Engine
{
	namespace Core
	{

		class MADGINE_BASE_EXPORT Root
		{
		public:
			Root(int argc = 0, char **argv = nullptr);
			~Root();

			int errorCode();

		private:

			int mErrorCode = 0;

			std::unique_ptr<CLI::CLI> mCLI;

#ifndef STATIC_BUILD
			std::unique_ptr<Plugins::PluginManager> mPluginManager;
			std::unique_ptr<UniqueComponentCollectorManager> mCollectorManager;
#endif
			std::unique_ptr<Scripting::LuaState> mLuaState;

#if ENABLE_MEMTRACKING
			std::unique_ptr<Debug::Memory::MemoryTracker> mMemTracker;
#endif
			
			std::unique_ptr<Resources::ResourceManager> mResources;			
		};

	}
}