#include "../debugtoolslib.h"

#include "debuggerview.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/debug/debugger.h"

#include "Meta/keyvalue/valuetype.h"

#include "Madgine_Tools/inspector/inspector.h"

UNIQUECOMPONENT(Engine::Tools::DebuggerView);

METATABLE_BEGIN_BASE(Engine::Tools::DebuggerView, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::DebuggerView)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DebuggerView, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::DebuggerView)

namespace Engine {
namespace Tools {

    DebuggerView::DebuggerView(ImRoot &root)
        : Tool<DebuggerView>(root)
        , mDebugger(Debug::Debugger::getSingleton())
    {
    }

    Threading::Task<bool> DebuggerView::init()
    {
        mInspector = &getTool<Inspector>();

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> DebuggerView::finalize()
    {

        co_await ToolBase::finalize();
    }

    void DebuggerView::render()
    {
        bool resume = false;

        if (ImGui::Begin("Debug Contexts")) {
            for (Debug::ContextInfo &info : mDebugger.infos()) {
                std::ostringstream descriptor;
                if (!info.mStack.empty())
                    descriptor << info.mStack.back().mAddress;
                if (!info.alive())
                    descriptor << " (dead)";
                else if (info.mPaused)
                    descriptor << " (paused)";
                if (ImGui::Selectable(descriptor.str().c_str(), mSelectedContext == &info)) {
                    mSelectedContext = &info;
                }
            }
        }
        ImGui::End();

        Debug::DebugLocation *prevSelected = mSelectedLocation;
        mSelectedLocation = nullptr;

        if (ImGui::Begin("Current Debug Context")) {
            if (!mSelectedContext) {
                ImGui::Text("No context selected!");
            } else if (!mSelectedContext->mPaused) {
                ImGui::Text("Context is not paused!");
            } else {
                if (!mSelectedContext->alive())
                    ImGui::BeginDisabled();
                if (ImGui::Button("Continue")) {
                    resume = true;
                }
                if (!mSelectedContext->alive())
                    ImGui::EndDisabled();

                for (Debug::FrameInfo &frame : mSelectedContext->mStack) {
                    if (frame.mLocation) {
                        if (!mSelectedLocation && prevSelected == frame.mLocation.get())
                            mSelectedLocation = frame.mLocation.get();
                        if (ImGui::Selectable(frame.mLocation->toString().c_str(), mSelectedLocation == frame.mLocation.get()))
                            mSelectedLocation = frame.mLocation.get();
                    } else
                        ImGui::Text("<Unknown>");
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Locals")) {
            if (!mSelectedContext) {
                ImGui::Text("No context selected!");
            } else if (!mSelectedContext->mPaused) {
                ImGui::Text("Context is not paused!");
            } else if (!mSelectedLocation) {
                ImGui::Text("No frame selected!");
            } else {
                if (ImGui::BeginTable("locals", 2, ImGuiTableFlags_Resizable)) {

                    for (auto& [key, value] : mSelectedLocation->localVariables()) {
                        ValueType v = value;
                        if (mInspector->drawValue(key, v, value.isReference()).first)
                            value = v;
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        if (resume)
            mSelectedContext->resume();
    }

    void DebuggerView::renderMenu()
    {
        ToolBase::renderMenu();
    }

    std::string_view DebuggerView::key() const
    {
        return "DebuggerView";
    }

}
}
