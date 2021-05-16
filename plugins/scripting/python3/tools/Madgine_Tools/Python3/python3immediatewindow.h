#pragma once

#include "toolbase.h"
#include "toolscollector.h"

namespace Engine {
namespace Tools {

    struct MADGINE_PYTHON3_TOOLS_EXPORT Python3ImmediateWindow : Tool<Python3ImmediateWindow> {
        Python3ImmediateWindow(ImRoot &root);

        virtual std::string_view key() const override;

        virtual bool init() override;
        virtual void finalize() override;

        virtual void renderMenu() override;

        virtual void render() override;

    private:
        std::string mCommandBuffer;
        std::stringstream mCommandLog;

        Scripting::Python3::Python3Environment *mEnv = nullptr;
    };

}
}

RegisterType(Engine::Tools::Python3ImmediateWindow);