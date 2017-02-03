#include "interfaceslib.h"
#include "assignment.h"
#include "Scripting/Types/scope.h"
#include "Scripting/scriptingexception.h"

#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {


Assignment::Assignment(int line, const std::string &varName, const std::string &op,
                       std::unique_ptr<const Statement> &&value) :
	Statement(line),
    mVarName(varName),
    mValue(std::forward<std::unique_ptr<const Statement>>(value))	
{
    if (varName == "my") throw ScriptingException(
            Exceptions::reservedKeyword("my"));

	if (op == "=") {
		mOp = ASSIGNMENT;
	}
	else if (op == "+=") {
		mOp = APPEND;
	}
	else if (op == "-=") {
		mOp = SUBTRACT;
	}
	else {
		throw ScriptingException(Exceptions::unknownOperator(op));
	}
}

ValueType Assignment::run(Scope *rootScope, Scope *scope, Stack &stack, bool *) const
{
    const ValueType &val = mValue->run(rootScope, 0, stack);
	ValueType &target = (scope ? scope->accessVar(mVarName) : stack[mVarName]);
    switch(mOp){
	case ASSIGNMENT:
		target = val;
		break;
	case APPEND:
		target += val;
		break;
	case SUBTRACT:
		target -= val;
		break;
	default:
        throw ScriptingException(Exceptions::unknownOperator(mOp));
    }
    return ValueType();
}

}
}
}
