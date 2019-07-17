#include "../toolslib.h"

#include "testtool.h"

#include "../imgui/imgui.h"

#include "../renderer/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

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
            ImGui::Text("  %s size: %d", name, size);
        }
    }

    void TestTool::render()
    {

        if (ImGui::Begin("TestTool")) {
            ImGui::Text("ValueType size: %d", sizeof(ValueType));
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

RegisterType(Engine::Tools::TestTool);