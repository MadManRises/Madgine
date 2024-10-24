#pragma once

#include "rootcomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

namespace Engine {
namespace Root {

    MADGINE_ROOT_EXPORT void skipUniqueComponentOnExport(const TypeInfo *t);
    MADGINE_ROOT_EXPORT void exportStaticComponentHeader(const Filesystem::Path &outFile, bool hasTools);

    struct MADGINE_ROOT_EXPORT Root {
        Root(int argc = 0, char **argv = nullptr);
        Root(std::unique_ptr<CLI::CLICore> cli);
        ~Root();

        int errorCode();

        
        template <typename T>
        T &getComponent()
        {
            return static_cast<T &>(getComponent(UniqueComponent::component_index<T>()));
        }

        RootComponentBase &getComponent(size_t i);

        static Root &getSingleton();

        bool toolMode() const;

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

        std::unique_ptr<RootComponentContainer<std::vector<Placeholder<0>>>> mComponents;
    };

}
}