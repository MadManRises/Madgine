#pragma once

#include "../toolscollector.h"
#include "../toolbase.h"
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

    protected:
        void refreshMethodCache();

        void parseMethods(TypedScopePtr scope);

    private:
        std::string mCurrentFunctionName;
        BoundApiFunction mCurrentFunction;
        ArgumentList mCurrentArguments;

        std::vector<std::pair<std::string, BoundApiFunction>> mMethodCache;
    };

}
}

RegisterType(Engine::Tools::FunctionTool);