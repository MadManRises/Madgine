#pragma once

#include "Madgine_Tools/toolscollector.h"

#include "Madgine_Tools/toolbase.h"

namespace Engine {
namespace Tools {

    struct DebuggerView : Tool<DebuggerView> {

        SERIALIZABLEUNIT(DebuggerView)

        DebuggerView(ImRoot &root);
        DebuggerView(const DebuggerView &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;        

        std::string_view key() const override;
        
    private:
        Debug::Debugger &mDebugger;
        Debug::ContextInfo *mSelectedContext = nullptr;
        Debug::DebugLocation *mSelectedLocation = nullptr;

        Inspector *mInspector = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::DebuggerView)