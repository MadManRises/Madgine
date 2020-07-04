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

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalueutil/keyvalueregistry.h"
#include "Modules/keyvalue/keyvaluescan.h"

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
        auto scan = [this](const std::map<TypedScopePtr, const char *> &items) {
            for (const std::pair<const TypedScopePtr, const char *> &p : items) {
                parseMethods(p.first);
            }
        };
        scan(KeyValueRegistry::globals());
        scan(KeyValueRegistry::workgroupLocals());
    }

    void FunctionTool::parseMethods(TypedScopePtr scope)
    {
        kvScan([this](const ApiFunction &m, std::string_view name, TypedScopePtr scope) {
            mMethodCache.emplace_back(std::move(name), BoundApiFunction { m, scope });
        },
            scope);
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

