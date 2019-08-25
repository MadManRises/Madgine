#pragma once

#include "../toolscollector.h"
#include "Modules/keyvalue/boundapimethod.h"
#include "Modules/keyvalue/valuetype.h"

namespace Engine {
namespace Tools {

    class MADGINE_TOOLS_EXPORT FunctionTool : public Tool<FunctionTool> {
    public:
        FunctionTool(ImGuiRoot &root);

        const char *key() const override;

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