#include "../toolslib.h"

#include "functiontool.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Madgine/app/application.h"

#include "Modules/keyvalue/keyvalueiterate.h"
#include "Modules/keyvalue/scopeiterator.h"

UNIQUECOMPONENT(Engine::Tools::FunctionTool);

namespace Engine {
namespace Tools {

    FunctionTool::FunctionTool(ImGuiRoot &root)
        : Tool<FunctionTool>(root)
    {
    }

    const char *FunctionTool::key() const
    {
        return "FunctionTool";
    }

    bool FunctionTool::init()
    {
        refreshMethodCache();
        return true;
    }

    void FunctionTool::setCurrentFunction(const std::string &name, const BoundApiMethod &method)
    {
        setVisible(true);
        mCurrentFunctionName = name;
        mCurrentFunction = method;
        mCurrentArguments.clear();
        mCurrentArguments.resize(method.argumentsCount());
    }

    void FunctionTool::refreshMethodCache()
    {
        mMethodCache.clear();
        parseMethods(&app());
    }

    void FunctionTool::parseMethods(TypedScopePtr scope)
    {
        for (std::pair<std::string, ValueType> p : scope) {
            struct Visitor {
                void operator()(BoundApiMethod &&m)
                {
                    mTool->mMethodCache.emplace_back(std::move(mName), m);
                }
                void operator()(TypedScopePtr &&child)
                {
                    mTool->parseMethods(child);
                }
                void operator()(KeyValueVirtualIterator &&it)
                {
                    for (; it != VirtualIteratorEnd {}; ++it) {
                        (*it).second.visit(*this);
                    }
                }

                void operator()(std::monostate &&) {}
                void operator()(std::string &&) {}
                void operator()(bool &&) {}
                void operator()(int &&) {}
                void operator()(size_t &&) {}
                void operator()(float &&) {}
                void operator()(InvScopePtr &&) {}
                void operator()(Matrix3 &&) {}
                void operator()(Quaternion &&) {}
                void operator()(Vector4 &&) {}
                void operator()(Vector3 &&) {}
                void operator()(Vector2 &&) {}
                void operator()(ObjectPtr &&) {}

                FunctionTool *mTool;
                std::string mName;
            };
            std::move(p.second).visit(Visitor { this, std::move(p.first) });
        }
    }

    void FunctionTool::render()
    {
        if (ImGui::Begin("FunctionTool", &mVisible)) {
            bool changed = ImGui::MethodPicker(nullptr, mMethodCache, &mCurrentFunction, &mCurrentFunctionName);
            if (ImGui::BeginDragDropTarget()) {
                const ImGui::ValueTypePayload *payload;
                if (ImGui::AcceptDraggableValueType(mCurrentFunction, &payload)) {
                    mCurrentFunctionName = payload->mName;
                    changed = true;
                }
                if (changed) {
                    mCurrentArguments.clear();
                    mCurrentArguments.resize(mCurrentFunction.argumentsCount());
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();
            ImGui::Text("(");
            //ImGui::SameLine();

            bool first = true;

            int i = 0;
            for (ValueType &arg : mCurrentArguments) {
                if (first) {
                    first = false;
                } else {
                    ImGui::SameLine();
                    ImGui::Text(", ");
                }
                ImGui::PushID(i++);
                ImGui::ValueType(&arg, true);
                if (ImGui::BeginDragDropTarget()) {
                    ImGui::AcceptDraggableValueType(arg);
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
                //ImGui::SameLine();
            }

            ImGui::Text(")");

            if (!mCurrentFunction.method())
                ImGui::PushDisabled();

            if (ImGui::Button("Call")) {
                mCurrentFunction(mCurrentArguments);
            }

            if (!mCurrentFunction.method())
                ImGui::PopDisabled();

            if (ImGui::Button("Refresh Cache")) {
                refreshMethodCache();
            }
        }
        ImGui::End();
    }
}
}

METATABLE_BEGIN(Engine::Tools::FunctionTool)
METATABLE_END(Engine::Tools::FunctionTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::FunctionTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::FunctionTool)

RegisterType(Engine::Tools::FunctionTool);