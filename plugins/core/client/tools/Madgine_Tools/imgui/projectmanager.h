#pragma once

#include "toolscollector.h"
#include "toolbase.h"

#include "Interfaces/filesystem/path.h"

#include "Modules/threading/signal.h"

namespace Engine {

namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ProjectManager : Tool<ProjectManager> {
        ProjectManager(ImRoot &root);

        virtual Threading::Task<bool> init() override;
        
        virtual std::string_view key() const override;

        virtual void renderMenu() override;

        const Filesystem::Path &projectRoot() const;
        const std::string &config() const;

        void save();
        void load();

#if ENABLE_PLUGINS
        void setProjectRoot(const Filesystem::Path &root);
        void setConfig(const std::string &config);

        std::vector<std::string> projectConfigs() const;

    private:
        Filesystem::Path mCurrentSelectionPath;
        Filesystem::Path mCurrentPath;
#endif

    private:
        Window::MainWindow *mWindow = nullptr;
        Filesystem::Path mProjectRoot;
        std::string mConfig;
    };

}
}

RegisterType(Engine::Tools::ProjectManager);