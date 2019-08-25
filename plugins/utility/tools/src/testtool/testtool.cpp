#include "../toolslib.h"

#include "testtool.h"

#include "../imgui/imgui.h"

#include "../renderer/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/reflection/classname.h"

#include "Madgine/app/application.h"
#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

UNIQUECOMPONENT(Engine::Tools::TestTool);

namespace Engine {
namespace Tools {

    TestTool::TestTool(ImGuiRoot &root)
        : Tool<TestTool>(root)
    {
    }

    template <size_t... Is>
    void renderValuetypeSizes(std::index_sequence<Is...>)
    {
        std::vector<std::pair<const char *, size_t>> data = {
            { typeid(std::get<Is>(std::declval<ValueType::Union>())).name(), sizeof(std::get<Is>(std::declval<ValueType::Union>())) }...
        };
        for (auto [name, size] : data) {
            ImGui::Text("  %s size: %lu", name, size);
        }
    }

    void TestTool::render()
    {

        if (ImGui::Begin("TestTool", &mVisible)) {
            if (ImGui::Button("Create Tool Window")) {
                Window::WindowSettings settings;
                app(false).getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->createToolWindow(settings);
            }            
			ImGui::DragFloat2("Scale", &ImGui::GetIO().DisplayFramebufferScale.x, 0.1f, 0.1f, 2.0f);

            ImGui::Text("ValueType size: %lu", sizeof(ValueType));
            renderValuetypeSizes(std::make_index_sequence<size_t(ValueType::Type::MAX_VALUETYPE_TYPE)>());
        }
        ImGui::End();
    }

    void TestTool::update()
    {

        ToolBase::update();
    }

    const char *TestTool::key() const
    {
        return "TestTool";
    }

    void TestTool::logTest()
    {
        LOG("Test");
    }

    void TestTool::logValue(const ValueType &v)
    {
        LOG(v.toString());
    }

}
}

METATABLE_BEGIN(Engine::Tools::TestTool)
FUNCTION(logTest)
FUNCTION(logValue)
METATABLE_END(Engine::Tools::TestTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::TestTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::TestTool)

RegisterType(Engine::Tools::TestTool);