#include "../toolslib.h"

#include "functiontool.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

//#include "Madgine/app/application.h"

#include "Modules/keyvalue/keyvaluevirtualiterator.h"
#include "Modules/keyvalue/scopeiterator.h"
#include "Modules/keyvalue/scopefield.h"

UNIQUECOMPONENT(Engine::Tools::FunctionTool);

namespace Engine {
namespace Tools {

    FunctionTool::FunctionTool(ImRoot &root)
        : Tool<FunctionTool>(root)
    {
    }

    std::string_view FunctionTool::key() const
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
        mVisible = true;
        mCurrentFunctionName = name;
        mCurrentFunction = method;
        mCurrentArguments.clear();
        mCurrentArguments.resize(method.argumentsCount());
    }

    void FunctionTool::refreshMethodCache()
    {
        mMethodCache.clear();
        //parseMethods(&app());
    }

    void FunctionTool::parseMethods(TypedScopePtr scope)
    {
        for (ScopeField f : scope) {
            struct Visitor {
                void operator()(const BoundApiMethod &m)
                {
                    mTool->mMethodCache.emplace_back(std::move(mName), m);
                }
                void operator()(const TypedScopePtr &child)
                {
                    mTool->parseMethods(child);
                }
                void operator()(KeyValueVirtualIterator it)
                {
                    for (; it != VirtualIteratorEnd {}; ++it) {
                        it->second.value().visit(*this);
                    }
                }

                void operator()(const std::monostate &) {}
                void operator()(const std::string &) {}
                void operator()(const std::string_view &) {}
                void operator()(const bool &) {}
                void operator()(const int &) {}
                void operator()(const size_t &) {}
                void operator()(const float &) {}
                void operator()(const Matrix3 &) {}
                void operator()(const Matrix4 &) {}
                void operator()(const Quaternion &) {}
                void operator()(const Vector4 &) {}
                void operator()(const Vector3 &) {}
                void operator()(const Vector2 &) {}
                void operator()(const ObjectPtr &) {}

                FunctionTool *mTool;
                std::string mName;
            };
            f.value().visit(Visitor { this, f.key() });
        }
    }

    void FunctionTool::render()
    {
        if (ImGui::Begin("FunctionTool", &mVisible)) {
            bool changed = ImGui::MethodPicker(nullptr, mMethodCache, &mCurrentFunction, &mCurrentFunctionName);
            if (ImGui::BeginDragDropTarget()) {
                const ImGui::ValueTypePayload *payload;
                ApiMethod function;
                if (ImGui::AcceptDraggableValueType(function, &payload)) {
                    mCurrentFunctionName = payload->mName;
                    mCurrentFunction = { function, payload->mSender };
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

            if (!mCurrentFunction.mMethod)
                ImGui::PushDisabled();

            if (ImGui::Button("Call")) {
                mCurrentFunction(mCurrentArguments);
            }

            if (!mCurrentFunction.mMethod)
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