#include "../toolslib.h"

#include "functiontool.h"

#include "../renderer/imroot.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

//#include "Madgine/app/application.h"

#include "inspector.h"

#include "Meta/keyvalue/keyvaluevirtualiterator.h"
#include "Meta/keyvalue/scopefield.h"
#include "Meta/keyvalue/scopeiterator.h"

#include "Meta/keyvalue/keyvaluepair.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/keyvaluescan.h"
#include "Madgine/core/keyvalueregistry.h"

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

    void FunctionTool::setCurrentFunction(const std::string &name, const BoundApiFunction &method)
    {
        mVisible = true;
        mCurrentFunctionName = name;
        mCurrentFunction = method;
        mCurrentArguments.clear();
        mCurrentArguments.resize(method.argumentsCount(true));
        for (size_t i = 0; i < method.argumentsCount(true); ++i) {
            if (method.mMethod.mTable->mArguments[i + method.isMemberFunction()].mType.mType.isRegular())
                mCurrentArguments[i].setType(method.mMethod.mTable->mArguments[i + method.isMemberFunction()].mType);
        }
    }

    bool FunctionTool::renderFunction(BoundApiFunction &function, const std::string &functionName, ArgumentList &args)
    {
        ImGui::Text(functionName);
        return renderFunctionDetails(function, args);
    }

    bool FunctionTool::renderFunctionSelect(BoundApiFunction &function, std::string &functionName, ArgumentList &args)
    {
        bool changed = ImGui::MethodPicker(nullptr, mMethodCache, &function, &functionName);
        if (ImGui::BeginDragDropTarget()) {
            const ImGui::ValueTypePayload *payload;
            ApiFunction f;
            if (ImGui::AcceptDraggableValueType(f, &payload)) {
                functionName = payload->mName;
                function = { f, payload->mSender };
                changed = true;
            }
            ImGui::EndDragDropTarget();
        }
        if (changed) {
            args.clear();
            args.resize(function.argumentsCount(true));
            for (size_t i = 0; i < function.argumentsCount(true); ++i) {
                args[i].setType(function.mMethod.mTable->mArguments[i + function.isMemberFunction()].mType);
            }
        }
        return renderFunctionDetails(function, args);
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

    bool FunctionTool::renderFunctionDetails(BoundApiFunction &function, ArgumentList &args)
    {
        if (function.mMethod) {

            ImGui::SameLine();
            ImGui::Text("(");
            //ImGui::SameLine();

            for (size_t i = 0; i < function.argumentsCount(); ++i) {

                ImGui::Text(function.mMethod.mTable->mArguments[i].mName);
                ImGui::SameLine();
                ImGui::Text(":");
                ImGui::SameLine();

                ImGui::PushID(i);
                ValueType v = i == 0 ? ValueType { function.mScope } : args[i - 1];
                bool changed = mInspector->drawValueImpl(nullptr, {}, "", v, true, !function.mMethod.mTable->mArguments[i].mType.mType.isRegular()).first;
                //ImGui::ValueType(&args[i], function.mMethod.mTable->mArguments[i].mType);
                if (ImGui::BeginDragDropTarget()) {
                    changed |= ImGui::AcceptDraggableValueType(v, function.mMethod.mTable->mArguments[i].mType);
                    ImGui::EndDragDropTarget();
                }
                if (changed) {
                    if (i == 0)
                        function.mScope = v.as<TypedScopePtr>();
                    else
                        args[i - 1] = v;
                }

                ImGui::PopID();
                ImGui::SameLine();
                ImGui::Text(function.mMethod.mTable->mArguments[i].mType.toString()); //TODO: Hover only
            }

            ImGui::Text(")");
        }

        if (!function.mMethod)
            ImGui::PushDisabled();

        bool call = false;

        if (ImGui::Button("Call")) {
            call = true;
        }

        if (!function.mMethod)
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
