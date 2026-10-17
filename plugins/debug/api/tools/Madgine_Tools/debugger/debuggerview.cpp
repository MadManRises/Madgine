#include "../debugtoolslib.h"

#include "debuggerview.h"

#include "Platform/debug/stacktrace.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/debug/debuggablelifetime.h"
#include "Madgine/debug/debuggablesender.h"
#include "Madgine/debug/debugger.h"
#include "Madgine/debug/statedescriptor.h"

#include "Meta/reflect/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine_Tools/imguiicons.h"
#include "Madgine_Tools/inspector/inspector.h"
#include "continuationlist.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

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

        mDebugger.addListener(this);

        mInspector->addPreviewDefinition<Debug::ContextInfo>([this](const Traced<Debug::ContextInfo *> &context) {
            renderDebugContext(*context.get());
            return false;
        });

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> DebuggerView::finalize()
    {
        mDebugger.removeListener(this);

        co_await ToolBase::finalize();
    }

    std::vector<TypedPtr> DebuggerView::visualizeDebugLocation(ContinuationList &continuations, TypedPtr location, TypedPtr inlineLocation)
    {
        if (const Debug::SenderLocation *senderLocation = location.as<const Debug::SenderLocation>()) {
            std::vector<TypedPtr> subLocations;

            size_t breakpointIndex = 0;
            bool lastWasText = false;

            auto visitor = [&, senderLocation, inlineLocation](auto &&val) {
                float startY = ImGui::GetCursorScreenPos().y;

                overloaded {
                    [&](const Execution::State::Text &text) {
                        ImGui::TextWrapped("%s", text.mText.c_str());
                    },
                    [&](const Execution::State::Progress &progress) {
                        if (progress.mInfinite)
                            ImGui::SpinnerEx(progress.mRatio, "Test", 5.0f, 0.1f, ImGui::GetColorU32(ImGuiCol_PlotHistogram));
                        else
                            ImGui::ProgressBar(progress.mRatio, ImVec2 { 100.0f, 10.0f }, "");
                    },
                    [&](const Execution::State::BeginBlock &begin) {
                        ImGui::BeginGroupPanel(begin.mName.data(), { -1, -1 }, begin.mCompleted ? IM_COL32(20, 255, 20, 15) : 0);
                    },
                    [&](const Execution::State::EndBlock &end) {
                        if (lastWasText)
                            ImGui::Dummy({ 0.0f, ImGui::GetStyle().ItemSpacing.y });
                        ImGui::EndGroupPanel();
                    },
                    [](const Execution::State::PushDisabled &) {
                        ImGui::BeginDisabled();
                    },
                    [](const Execution::State::PopDisabled &) {
                        ImGui::EndDisabled();
                    },
                    [&, inlineLocation](const Execution::State::DebugLocation &subLoc) {
                        std::ranges::move(visualizeDebugLocation(continuations, subLoc.mLocation, inlineLocation), std::back_inserter(subLocations));
                        if (startY == ImGui::GetCursorScreenPos().y)
                            ImGui::Dummy({
                                0.0f,
                                0.1f,
                            });
                    },
                    [&](const Execution::State::Breakpoint &bp) {
                        if (bp.mContinuation && *bp.mContinuation) {

                            std::stringstream ss;
                            bp.mContinuation->visitArguments(ss);
                            std::string arguments = ss.str();
                            if (!arguments.empty()) {
                                Debug::ContinuationType type = bp.mContinuation->type();
                                switch (type) {
                                case Debug::ContinuationType::Done:
                                case Debug::ContinuationType::Error:
                                    ImGui::PushStyleColor(ImGuiCol_Border, { 1.0f, 0.0f, 0.0f, 1.0f });
                                    // ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
                                    break;
                                case Debug::ContinuationType::Return:
                                    ImGui::PushStyleColor(ImGuiCol_Border, { 0.0f, 0.78f, 1.0f, 1.0f });
                                    // ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.78f, 1.0f, 1.0f });
                                    break;
                                case Debug::ContinuationType::Flow:
                                    break;
                                case Debug::ContinuationType::Cancelling: // TODO
                                    break;
                                }
                                ImGui::BeginGroupPanel();
                                ImGui::Text(arguments);
                                ImGui::EndGroupPanel();
                                if (type != Debug::ContinuationType::Flow && type != Debug::ContinuationType::Cancelling) {
                                    ImGui::PopStyleColor(1);
                                }
                            }
                        }
                        continuations.lazyBreakpoint(bp, breakpointIndex++, location);
                    },
                    [&](const Execution::State::Marker &m) {
                        continuations.marker();
                    },
                    [](const Execution::State::FunctionPtr &f) {
                        const char *name = "function";
                        const char *details = f.mTypeName;
#ifndef NDEBUG
                        Debug::TraceBack trace = Debug::resolveSymbols(&f.mFunctionPtr, 1)[0];
                        std::string detailsStr = std::string { trace.mFile } + ":" + std::to_string(trace.mLineNr);

                        name = trace.mFunction;
                        details = detailsStr.c_str();
#endif

                        ImGui::Text("%s", name);
                        ImGui::SetItemTooltip("%s", details);
                    },
                    [this](const Execution::State::Value &v) {
                        if (ImGui::BeginTable("columns", 2)) {
                            UndoStack stack;
                            TracedRoot<Reflect::Value> value { stack };
                            v.get(value.get());
                            bool changed = mInspector->drawValue(v.mName, value, true, v.mType);
                            if (changed)
                                v.set(value.get());
                            ImGui::EndTable();
                        }
                    }
                }(std::forward<decltype(val)>(val));

                if (startY != ImGui::GetCursorScreenPos().y) {
                    lastWasText = std::same_as<std::decay_t<decltype(val)>, Execution::State::Text>;
                    continuations.flush(startY, ImGui::GetCursorScreenPos().y);
                }
            };
            senderLocation->visit(visitor);

            return subLocations;
        } else {
            auto it = mDebugLocationVisualizers.find(location.type());
            if (it != mDebugLocationVisualizers.end()) {
                return it->second(continuations, *this, location.ptr(), inlineLocation);
            }
            ImGui::TextWrapped("Unknown [%s]", location.type().name());
            return {};
        }
    }

    ControlButton DebuggerView::contextControls(bool running)
    {
        ControlButton button = ControlButton::NONE;
        if (running)
            ImGui::BeginDisabled();
        if (ImGui::Button(IMGUI_ICON_PLAY)) {
            button = ControlButton::PLAY;
        }
        ImGui::SameLine(0, 0);
        if (ImGui::Button(IMGUI_ICON_STEP)) {
            button = ControlButton::STEPOVER;
        }
        ImGui::SameLine(0, 0);
        if (ImGui::Button(IMGUI_ICON_STEP)) {
            button = ControlButton::STEPINTO;
        }
        if (running)
            ImGui::EndDisabled();
        else
            ImGui::BeginDisabled();
        ImGui::SameLine(0, 0);
        if (ImGui::Button(IMGUI_ICON_PAUSE)) {
            button = ControlButton::PAUSE;
        }
        if (!running)
            ImGui::EndDisabled();
        ImGui::SameLine(0, 0);
        if (ImGui::Button(IMGUI_ICON_STOP)) {
            button = ControlButton::STOP;
        }

        return button;
    }

    void DebuggerView::render()
    {
        if (beginToolWindow("Debugger", &mVisible)) {

            if (beginSubPanel("Debug Contexts", nullptr, ImGuiDir_Left)) {
                std::unique_lock lock { mDebugger.mMutex };
                for (Debug::ContextInfo &info : mDebugger.infos()) {
                    std::ostringstream descriptor;
                    if (info.alive()) {
                        descriptor << "Context";
                        if (info.isPaused()) {
                            descriptor << " (paused)";
                        }
                    } else {
                        descriptor << " (dead)";
                    }

                    descriptor << "###Context";
                    ImGui::PushID(&info);
                    if (ImGui::Selectable(descriptor.str().c_str(), mSelectedContext == &info)) {
                        setCurrentContext(info);
                    }
                    ImGui::PopID();
                }
            }
            ImGui::End();

            mSelectedLocation = nullptr;

            if (beginContent()) {
                if (!mSelectedContext) {
                    ImGui::Text("No context selected!");
                } else {
                    renderDebugContext(*mSelectedContext);
                }
            }
            ImGui::End();

            if (beginSubPanel("Locals", nullptr, ImGuiDir_Down)) {
                if (!mSelectedContext) {
                    ImGui::Text("No context selected!");
                } else if (!mSelectedContext->isPaused()) {
                    ImGui::Text("Context is not paused!");
                } else if (!mSelectedLocation) {
                    ImGui::Text("No frame selected!");
                } else {
                    if (ImGui::BeginTable("locals", 2, ImGuiTableFlags_Resizable)) {

                        /* for (auto &[key, value] : mSelectedLocation->localVariables()) {
                            ValueType v = value;
                            if (mInspector->drawValue(key, v, value.isReference(), value.type()).first)
                                value = v;
                        }*/
                        ImGui::Text("TODO");

                        ImGui::EndTable();
                    }
                }
            }
            ImGui::End();
        }
        ImGui::End();
    }

    void DebuggerView::renderMenu()
    {
        ToolBase::renderMenu();
    }

    void DebuggerView::renderDebugContext(const Debug::ContextInfo &context)
    {
        ControlButton control = contextControls(!context.isPaused());
        ContinuationList continuations { context, control };

        std::unique_lock guard { context.mMutex };
        if (context.mChild) {
            if (BeginDebuggablePanel("Debug Context")) {
                [[maybe_unused]] std::vector<TypedPtr> children = visualizeDebugLocation(continuations, context.mChild, {});
                assert(children.empty()); // Parents that allow inline rendering need to take care of child rendering.
                continuations.flush(ImGui::GetCursorScreenPos().y - 10.0f, ImGui::GetCursorScreenPos().y);
                EndDebuggablePanel();
            }
        }
    }

    void DebuggerView::renderLifetime(Debug::DebuggableLifetimeBase &lifetime)
    {
        for (Debug::ContextInfo &context : lifetime.debugContexts()) {
            renderDebugContext(context);
        }
    }

    void DebuggerView::setCurrentContext(Debug::ContextInfo &context)
    {
        mSelectedContext = &context;
    }

    void DebuggerView::onSuspend(Debug::ContextInfo &context, TypedPtr location, Debug::ContinuationType type)
    {
        setCurrentContext(context);
    }

    bool DebuggerView::wantsPause(Debug::ContextInfo &context, TypedPtr location, Debug::ContinuationType type, IndexType<size_t> line)
    {
        return false;
    }

    std::string_view DebuggerView::key() const
    {
        return "DebuggerView";
    }

    static std::vector<float> sDebugStartX;

    bool BeginDebuggablePanel(const char *name)
    {
        sDebugStartX.push_back(ImGui::GetCursorScreenPos().x);
        ImGui::Indent();
        return true;
    }

    void EndDebuggablePanel()
    {
        ImGui::Unindent();
        sDebugStartX.pop_back();
    }

    void DrawDebugMarker(float y)
    {
        ImGui::PushClipRect({ -1000, -1000 }, { 10000, 10000 }, false);
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        float x = sDebugStartX.back();
        draw_list->AddRectFilled({ x + 3.0f, y - 2.0f }, { x + 12.0f, y + 3.0f }, IM_COL32(255, 200, 10, 255));
        draw_list->AddTriangleFilled({ x + 12.0f, y - 5.0f }, { x + 12.0f, y + 5.0f }, { x + 17.0f, y }, IM_COL32(255, 200, 10, 255));
        ImGui::PopClipRect();
    }

    bool Breakpoint(float startY, float endY, bool *set)
    {
        ImGui::PushClipRect({ -1000, -1000 }, { 10000, 10000 }, false);
        const float radius = 7.0f;
        float x = sDebugStartX.back() + radius + 3.0f;
        float y = 0.5f * (endY + startY);

        bool hovered = false;

        bool clicked = ImGui::ButtonBehavior({ { x - radius, std::min(y - radius, startY) }, { x + radius, std::max(y + radius, endY) } }, ImGui::GetID(set), &hovered, nullptr, ImGuiButtonFlags_PressedOnClick);

        if (clicked && set)
            *set = !*set;

        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        if (!set || !*set) {
            if (hovered) {
                draw_list->AddCircleFilled({ x, y }, 7.0f, IM_COL32(155, 55, 55, 55));
                draw_list->AddCircle({ x, y }, 7.0f, IM_COL32(155, 155, 155, 155));
            }
        } else {
            draw_list->AddCircleFilled({ x, y }, 7.0f, IM_COL32(255, 100, 100, 255));
        }

        ImGui::PopClipRect();

        return clicked;
    }

    void Engine::Tools::ErrorMarker(float startY, float endY)
    {
        ImGui::GetCurrentWindow()->DrawList->AddRect({ sDebugStartX.back() - 4.0f, startY - 1 }, { sDebugStartX.back() + 4.0f, endY + 1 }, IM_COL32(255, 0, 0, 255), 0.0f, ImDrawFlags_None, 2.0f);
    }

}
}
