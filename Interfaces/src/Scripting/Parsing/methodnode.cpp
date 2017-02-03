#include "interfaceslib.h"
#include "methodnode.h"
#include "valuetype.h"
#include "Scripting/Statements/statement.h"
#include "Scripting/scriptingexception.h"
#include "Scripting\Types\scope.h"
#include "Util\UtilMethods.h"

namespace Engine {
namespace Scripting {
namespace Parsing {


ValueType MethodNode::run(Scope *scope, Stack &stack) const
{
    bool bReturn = false;
	for (const std::unique_ptr<const Statements::Statement> &s : mStatements) {
		TRACE_S(getOrigin(), s->getLine(), mName);
		ValueType temp = s->run(scope, 0, stack, &bReturn);
		if (bReturn)
			return temp;
	}
    return ValueType();
}

void MethodNode::addArgument(const std::string &arg)
{
    mArguments.push_back(arg);
}

void MethodNode::addStatement(std::unique_ptr<const Statements::Statement> &&s)
{
    mStatements.emplace_back(std::forward<std::unique_ptr<const Statements::Statement>>(s));
}

const std::list<std::string> &MethodNode::arguments() const
{
    return mArguments;
}

void MethodNode::clear()
{
	mStatements.clear();
	mArguments.clear();
}

}
}
}
