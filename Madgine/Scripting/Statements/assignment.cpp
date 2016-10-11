#include "libinclude.h"
#include "assignment.h"
#include "Scripting/Types/scope.h"
#include "Scripting/scriptexception.h"
#include "Scripting/Datatypes/varset.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {


Assignment::Assignment(int line, const std::string &varName, const std::string &op,
                       Ogre::unique_ptr<const Statement> &&value):
    mVarName(varName),
    mValue(std::forward<Ogre::unique_ptr<const Statement>>(value)),
	Statement(line)
{
    if (varName == "my") throw ScriptingException(
            Database::Exceptions::reservedKeyword("my"));

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
		throw ScriptingException(Database::Exceptions::unknownOperator(op));
	}
}

ValueType Assignment::run(Scope *rootScope, Scope *scope, VarSet &stack, bool *) const
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
        throw ScriptingException(Database::Exceptions::unknownOperator(mOp));
    }
    return ValueType();
}

}
}
}
