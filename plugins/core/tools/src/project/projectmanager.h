#pragma once

#include "../toolscollector.h"

#include "Interfaces/filesystem/path.h"

#include "Modules/threading/signal.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ProjectManager : Tool<ProjectManager> {
        ProjectManager(ImRoot &root);

        virtual std::string_view key() const override;

        virtual void renderMenu() override;

        const Filesystem::Path &projectRoot() const;
        const std::string &config() const;

#if ENABLE_PLUGINS
        void setProjectRoot(const Filesystem::Path &root);
        void setConfig(const std::string &config);

        std::vector<std::string> projectConfigs() const;

        Threading::Signal<const Filesystem::Path &, const std::string &> mProjectChanged;

    private:
        Filesystem::Path mCurrentSelectionPath;
        Filesystem::Path mCurrentPath;
#endif

    private:
        Filesystem::Path mProjectRoot;
        std::string mConfig;
    };

}
}