#include "interfaceslib.h"
#include "methodcall.h"
#include "Scripting/Types/scope.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {


MethodCall::MethodCall(int line, const std::string &methodName,
                       std::list<std::unique_ptr<const Statement>> &&arguments) :
	Statement(line),
	mMethodName(methodName),
    mArguments(std::forward<std::list<std::unique_ptr<const Statement>>>(arguments))	
{

}

ValueType MethodCall::run(Scope *rootScope, Scope *scope, Stack &stack, bool *) const
{
    if (!scope) scope = rootScope;
    ArgumentList args(mArguments.size());
    unsigned int i = 0;
    for (const std::unique_ptr<const Statement> &s : mArguments) {
        args[i++] = s->run(rootScope, 0, stack);
    }
    return scope->methodCall(mMethodName, args);

}

}
}
}
