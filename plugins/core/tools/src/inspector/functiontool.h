#pragma once

#include "../toolbase.h"
#include "../toolscollector.h"
#include "Modules/keyvalue/boundapifunction.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT FunctionTool : Tool<FunctionTool> {
        FunctionTool(ImRoot &root);
        ~FunctionTool();

        std::string_view key() const override;

        virtual bool init() override;

        virtual void render() override;

        void setCurrentFunction(const std::string &name, const BoundApiFunction &method);

        bool renderFunction(BoundApiFunction &function, const std::string &functionName, ArgumentList &args);
        bool renderFunctionSelect(BoundApiFunction &function, std::string &functionName, ArgumentList &args);

    protected:
        void refreshMethodCache();

        void parseMethods(TypedScopePtr scope);

        bool renderFunctionDetails(BoundApiFunction &function, ArgumentList &args);

    private:
        std::string mCurrentFunctionName;
        BoundApiFunction mCurrentFunction;
        ArgumentList mCurrentArguments;

        std::vector<std::pair<std::string, BoundApiFunction>> mMethodCache;

        Inspector *mInspector;
    };

}
}

RegisterType(Engine::Tools::FunctionTool);