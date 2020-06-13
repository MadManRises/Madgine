#include "../toolslib.h"

#include "functiontool.h"

#include "../renderer/imroot.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

//#include "Madgine/app/application.h"

#include "Modules/keyvalue/keyvaluevirtualiterator.h"
#include "Modules/keyvalue/scopefield.h"
#include "Modules/keyvalue/scopeiterator.h"

#include "Modules/keyvalue/keyvaluepair.h"

UNIQUECOMPONENT(Engine::Tools::FunctionTool);

namespace Engine {
namespace Tools {

    FunctionTool::FunctionTool(ImRoot &root)
        : Tool<FunctionTool>(root)
    {
    }

    FunctionTool::~FunctionTool()
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

    void FunctionTool::setCurrentFunction(const std::string &name, const BoundApiFunction &method)
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
        //parseMethods(mRoot.toolsTopLevel());
    }

    void FunctionTool::parseMethods(TypedScopePtr scope)
    {
        for (ScopeField f : scope) {
            struct Visitor {
                void operator()(const ApiFunction &m)
                {
                    mTool->mMethodCache.emplace_back(std::move(mName), BoundApiFunction { m, mScope });
                }
                void operator()(const TypedScopePtr &child)
                {
                    mTool->parseMethods(child);
                }
                void operator()(KeyValueVirtualIterator it)
                {
                    for (; it != VirtualIteratorEnd {}; ++it) {
                        it->mValue.value().visit(*this);
                    }
                }

                void operator()(const std::monostate &) {}
                void operator()(const CoWString &) {}
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
                TypedScopePtr mScope;
            };
            f.value().visit(Visitor { this, f.key(), scope });
        }
    }

    void FunctionTool::render()
    {
        if (ImGui::Begin("FunctionTool", &mVisible)) {
            bool changed = ImGui::MethodPicker(nullptr, mMethodCache, &mCurrentFunction, &mCurrentFunctionName);
            if (ImGui::BeginDragDropTarget()) {
                const ImGui::ValueTypePayload *payload;
                ApiFunction function;
                if (ImGui::AcceptDraggableValueType(function, &payload)) {
                    mCurrentFunctionName = payload->mName;
                    mCurrentFunction = { function, payload->mSender };
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }
            if (changed) {
                mCurrentArguments.clear();
                mCurrentArguments.resize(mCurrentFunction.argumentsCount());
                if (mCurrentFunction.isMemberFunction()) {
                    mCurrentArguments[0] = mCurrentFunction.mScope;
                }
            }

            if (mCurrentFunction.mMethod) {

                ImGui::SameLine();
                ImGui::Text("(");
                //ImGui::SameLine();

                for (size_t i = 0; i < mCurrentFunction.argumentsCount(); ++i) {

                    ImGui::Text(mCurrentFunction.mMethod.mTable->mArguments[i].mName);
                    ImGui::SameLine();
                    ImGui::Text(":");
                    ImGui::SameLine();

                    ImGui::PushID(i);
                    ImGui::ValueType(&mCurrentArguments[i], mCurrentFunction.mMethod.mTable->mArguments[i].mType);
                    if (ImGui::BeginDragDropTarget()) {
                        ImGui::AcceptDraggableValueType(mCurrentArguments[i], mCurrentFunction.mMethod.mTable->mArguments[i].mType);
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::Text(mCurrentFunction.mMethod.mTable->mArguments[i].mType.toString());
                }

                ImGui::Text(")");
            }

            if (!mCurrentFunction.mMethod)
                ImGui::PushDisabled();

            if (ImGui::Button("Call")) {
                ValueType result;
                mCurrentFunction.mMethod(result, mCurrentArguments);
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

