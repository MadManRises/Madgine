#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

namespace Engine {
namespace Tools {

    struct MADGINE_PYTHON3_TOOLS_EXPORT Python3ImmediateWindow : Tool<Python3ImmediateWindow> {
        Python3ImmediateWindow(ImRoot &root);

        virtual std::string_view key() const override;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        virtual void render() override;

    private:
        std::string mCommandBuffer;
        std::ostringstream mCommandLog;

        Scripting::Python3::Python3Environment *mEnv = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::Python3ImmediateWindow)