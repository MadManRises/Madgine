#include "../toolslib.h"

#include "Meta/inject/variable.h"
#include "Meta/inject/inject.h"
#include "controlflow.h"

#include "injectortool.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include <imgui/imgui.h>

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

UNIQUECOMPONENT(Engine::Tools::InjectorTool);


METATABLE_BEGIN_BASE(Engine::Tools::InjectorTool, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::InjectorTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::InjectorTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::InjectorTool)


namespace Engine {
namespace Tools {

    template <typename... Injectors>
    Inject::Variable<int, Injectors...> sample()
    {
        Inject::Variable<int, Injectors...> v = 3;
        Inject::Variable<int, Injectors...> v2 = 3;
        while (v2 > 0) {
            ++v;
            --v2;
        }
        return v;
    }

    InjectorTool::InjectorTool(ImRoot &root)
        : Tool<InjectorTool>(root)
    {
        auto result = Inject::inject<Inject::ControlFlow>(&sample<Inject::ControlFlow>);

        mGraph = result->mContext.graph();
    }

    InjectorTool::~InjectorTool()
    {
    }

    void InjectorTool::render()
    {
        if (ImGui::Begin("InjectorTool", &mVisible)) {
            for (const Inject::ControlFlow::BasicBlock& block : mGraph) {
                ImGui::BeginChild(ImGui::GetID(&block));
                ImGui::Text("Block");
                ImGui::Text("Range: %p - %p", block.mAddresses.front(), block.mAddresses.back());
                std::stringstream ss;
                for (size_t i : block.mTargets)
                    ss << " " << i;
                ImGui::Text(("Targets:" + ss.str()).c_str());
                ImGui::EndChild();                
            }
        }
        ImGui::End();
    }

    std::string_view InjectorTool::key() const {
        return "InjectorTool";
    }

}
}
