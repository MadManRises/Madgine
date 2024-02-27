#include "behaviortoolslib.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine_Tools/imguiicons.h"

#include "Madgine_Tools/debugger/debuggerview.h"

#include "behaviortool.h"

#include "Madgine/behavior.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "Madgine/behaviorcollector.h"

UNIQUECOMPONENT(Engine::Tools::BehaviorTool);

METATABLE_BEGIN_BASE(Engine::Tools::BehaviorTool, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::BehaviorTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::BehaviorTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::BehaviorTool)

namespace Engine {
namespace Tools {

    void visualizeCoroutineLocation(DebuggerView *view, const Debug::ContextInfo *context, const CoroutineLocation *location)
    {
        const char *name;
        Debug::FullStackTrace trace = location->mStacktrace.calculateReadable();
        if (trace.empty()) {
            name = "<unknown>";
        } else {
            name = trace[0].mFunction;
        }
        ImGui::BeginGroupPanel(name);
        view->visualizeDebugLocation(context, location->mChild);
        ImGui::EndGroupPanel();
    }

    BehaviorTool::BehaviorTool(ImRoot &root)
        : Tool<BehaviorTool>(root)
    {
    }

    Threading::Task<bool> BehaviorTool::init()
    {
        getTool<DebuggerView>().registerDebugLocationVisualizer<visualizeCoroutineLocation>();

        co_return co_await ToolBase::init();
    }

    std::string_view BehaviorTool::key() const
    {
        return "BehaviorTool";
    }

    Threading::Task<void> BehaviorTool::finalize()
    {
        co_await ToolBase::finalize();
    }

}
}
