#pragma once

namespace Engine {
namespace Core {

    MADGINE_BASE_EXPORT void skipUniqueComponentOnExport(const TypeInfo *t);
    MADGINE_BASE_EXPORT void exportStaticComponentHeader(const Filesystem::Path &outFile, bool hasTools);

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
        std::unique_ptr<UniqueComponent::CollectorManager> mCollectorManager;
#endif

#if ENABLE_MEMTRACKING
        std::unique_ptr<Debug::Memory::MemoryTracker> mMemTracker;
#endif

        std::unique_ptr<Resources::ResourceManager> mResources;
    };

}
}