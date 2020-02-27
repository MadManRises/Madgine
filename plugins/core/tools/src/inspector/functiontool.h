#pragma once

#include "../toolscollector.h"
#include "Modules/keyvalue/boundapimethod.h"
#include "Modules/keyvalue/valuetype.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT FunctionTool : Tool<FunctionTool> {
        FunctionTool(ImRoot &root);

        std::string_view key() const override;

        virtual bool init() override;

        virtual void render() override;

        void setCurrentFunction(const std::string &name, const BoundApiMethod &method);

    protected:
        void refreshMethodCache();

        void parseMethods(TypedScopePtr scope);

    private:
        std::string mCurrentFunctionName;
        BoundApiMethod mCurrentFunction;
        ArgumentList mCurrentArguments;

        std::vector<std::pair<std::string, BoundApiMethod>> mMethodCache;
    };

}
}