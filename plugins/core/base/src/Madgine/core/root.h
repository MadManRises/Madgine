#pragma once

namespace Engine {
namespace Core {

    struct MADGINE_BASE_EXPORT Root {
        Root(int argc = 0, char **argv = nullptr);
        Root(std::unique_ptr<CLI::CLICore> cli);
        ~Root();

        int errorCode();

    private:
        int mErrorCode = 0;

        std::unique_ptr<CLI::CLICore> mCLI;

#if ENABLE_PLUGINS
        std::unique_ptr<Plugins::PluginManager> mPluginManager;
        std::unique_ptr<UniqueComponentCollectorManager> mCollectorManager;
#endif

#if ENABLE_MEMTRACKING
        std::unique_ptr<Debug::Memory::MemoryTracker> mMemTracker;
#endif

        std::unique_ptr<Resources::ResourceManager> mResources;
    };

}
}