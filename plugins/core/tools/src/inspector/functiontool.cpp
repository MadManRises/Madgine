#include "../toolslib.h"

#include "functiontool.h"

#include "../renderer/imroot.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

//#include "Madgine/app/application.h"

#include "inspector.h"

#include "Meta/keyvalue/keyvaluevirtualrange.h"
#include "Meta/keyvalue/scopefield.h"
#include "Meta/keyvalue/scopeiterator.h"

#include "Meta/keyvalue/keyvaluepair.h"
#include "Meta/keyvalue/functionargument.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/keyvaluescan.h"
#include "Madgine/core/keyvalueregistry.h"

#include "imgui/valuetypepayload.h"

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
        mInspector = &mRoot.getTool<Inspector>();
        refreshMethodCache();
        return true;
    }

    void FunctionTool::setCurrentFunction(std::string_view name, const BoundApiFunction &method)
    {
        mVisible = true;
        mCurrentFunctionName = name;
        mCurrentFunction = method;
        mCurrentArguments.clear();
        mCurrentArguments.resize(method.argumentsCount(true));
        for (size_t i = 0; i < method.argumentsCount(true); ++i) {
            if (method.mFunction.mTable->mArguments[i + method.isMemberFunction()].mType.mType.isRegular())
                mCurrentArguments[i].setType(method.mFunction.mTable->mArguments[i + method.isMemberFunction()].mType);
        }
    }

    bool FunctionTool::renderFunction(BoundApiFunction &function, std::string_view functionName, ArgumentList &args)
    {
        ImGui::Text(functionName);
        return renderFunctionDetails(function, args);
    }

    bool FunctionTool::renderFunctionSelect(BoundApiFunction &function, std::string &functionName, ArgumentList &args)
    {
        bool changed = ImGui::MethodPicker(nullptr, mMethodCache, &function, &functionName);
        if (ImGui::BeginDragDropTarget()) {
            const ImGui::ValueTypePayload *payload;
            if (ImGui::AcceptDraggableValueType(function, &payload)) {
                functionName = payload->mName;
                changed = true;
            }
            ImGui::EndDragDropTarget();
        }
        if (changed) {
            args.clear();
            args.resize(function.argumentsCount(true));
            for (size_t i = 0; i < function.argumentsCount(true); ++i) {
                args[i].setType(function.mFunction.mTable->mArguments[i + function.isMemberFunction()].mType);
            }
        }
        return renderFunctionDetails(function, args);
    }

    void FunctionTool::refreshMethodCache()
    {
        mMethodCache.clear();
        auto scan = [this](const std::map<std::string_view, TypedScopePtr> &items) {
            for (const std::pair<const std::string_view, TypedScopePtr> &p : items) {
                parseMethods(p.second);
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

    bool FunctionTool::renderFunctionDetails(BoundApiFunction &function, ArgumentList &args)
    {
        if (function.mFunction) {

            ImGui::SameLine();
            ImGui::Text("(");
            //ImGui::SameLine();

            for (size_t i = 0; i < function.argumentsCount(); ++i) {

                ImGui::Text(function.mFunction.mTable->mArguments[i].mName);
                ImGui::SameLine();
                ImGui::Text(":");
                ImGui::SameLine();

                ImGui::PushID(i);
                ValueType v = i == 0 ? ValueType { function.scope() } : args[i - 1];
                bool changed = mInspector->drawValue("", v, true, !function.mFunction.mTable->mArguments[i].mType.mType.isRegular()).first;
                //ImGui::ValueType(&args[i], function.mMethod.mTable->mArguments[i].mType);
                if (ImGui::BeginDragDropTarget()) {
                    changed |= ImGui::AcceptDraggableValueType(v, function.mFunction.mTable->mArguments[i].mType);
                    ImGui::EndDragDropTarget();
                }
                if (changed) {
                    if (i == 0)
                        function.mScope = v.as<TypedScopePtr>().mScope;
                    else
                        args[i - 1] = v;
                }

                ImGui::PopID();
                if(ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", function.mFunction.mTable->mArguments[i].mType.toString().c_str());
            }

            ImGui::Text(")");
        }

        if (!function.mFunction)
            ImGui::PushDisabled();

        bool call = false;

        if (ImGui::Button("Call")) {
            call = true;
        }

        if (!function.mFunction)
            ImGui::PopDisabled();

        return call;
    }

    void FunctionTool::render()
    {
        if (ImGui::Begin("FunctionTool", &mVisible)) {
            if (renderFunctionSelect(mCurrentFunction, mCurrentFunctionName, mCurrentArguments)) {
                ValueType result;
                mCurrentFunction(result, mCurrentArguments);
            }

            if (ImGui::Button("Refresh Cache")) {
                refreshMethodCache();
            }
        }
        ImGui::End();
    }
}
}

METATABLE_BEGIN_BASE(Engine::Tools::FunctionTool, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::FunctionTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::FunctionTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::FunctionTool)
