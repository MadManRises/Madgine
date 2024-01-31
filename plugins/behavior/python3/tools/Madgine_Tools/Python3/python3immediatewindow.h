#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

#include "Madgine/debug/debugger.h"

#include "Madgine_Tools/texteditor/interactiveprompt.h"

namespace Engine {
namespace Tools {

    struct MADGINE_PYTHON3_TOOLS_EXPORT Python3ImmediateWindow : Tool<Python3ImmediateWindow>, Debug::DebugListener, Interpreter {
        Python3ImmediateWindow(ImRoot &root);

        virtual std::string_view key() const override;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        virtual void render() override;

    protected:
        bool pass(Debug::DebugLocation *location) override;
        void onSuspend(Debug::ContextInfo &context) override;

        bool interpret(std::string_view command) override;

    private:
        std::string mCommandBuffer;
        std::ostringstream mCommandLog;

        std::unique_ptr<InteractivePrompt> mPrompt;

        Scripting::Python3::Python3Environment *mEnv = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::Python3ImmediateWindow)