#pragma once

#include "Madgine_Tools/toolscollector.h"

#include "Madgine_Tools/toolbase.h"

#include "Madgine/debug/debugger.h"

struct ImVec2;
struct ImDrawList;

namespace Engine {
namespace Tools {

    bool BeginDebuggablePanel(const char *name);
    void EndDebuggablePanel();
    MADGINE_DEBUGGER_TOOLS_EXPORT void DrawDebugMarker(float y);

    struct MADGINE_DEBUGGER_TOOLS_EXPORT DebuggerView : Tool<DebuggerView>, Debug::DebugListener {

        SERIALIZABLEUNIT(DebuggerView)

        DebuggerView(ImRoot &root);
        DebuggerView(const DebuggerView &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        void renderDebugContext(const Debug::ContextInfo *context);

        void setCurrentContext(Debug::ContextInfo &context);

        void onSuspend(Debug::ContextInfo &context, Debug::ContinuationType type) override;
        bool pass(Debug::DebugLocation *location, Debug::ContinuationType type) override;

        std::string_view key() const override;

        template <auto Visualizer>
        void registerDebugLocationVisualizer()
        {
            using T = typename CallableTraits<decltype(Visualizer)>::argument_types::template select<2>;
            mDebugLocationVisualizers.push_back([](DebuggerView *view, const Debug::ContextInfo *context, const Debug::DebugLocation *location) {
                T typedLocation = dynamic_cast<T>(location);
                if (typedLocation)
                    Visualizer(view, context, typedLocation);
                return static_cast<bool>(typedLocation);
            });
        }

        void visualizeDebugLocation(const Debug::ContextInfo *context, const Debug::DebugLocation *location);

        Debug::ContinuationMode contextControls(Debug::ContextInfo &context);

    private:
        Debug::Debugger &mDebugger;
        Debug::ContextInfo *mSelectedContext = nullptr;
        Debug::DebugLocation *mSelectedLocation = nullptr;

        std::vector<bool (*)(DebuggerView*, const Debug::ContextInfo *, const Debug::DebugLocation *)> mDebugLocationVisualizers;

        Inspector *mInspector = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::DebuggerView)