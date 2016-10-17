#include "libinclude.h"
#include "comparator.h"
#include "Scripting/scriptexception.h"
#include "Scripting/Datatypes/valuetype.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {


Comparator::Comparator(int line, const std::string &op, Ogre::unique_ptr<const Statement> &&first,
                                 Ogre::unique_ptr<const Statement> &&second) :
    mFirst(std::forward<Ogre::unique_ptr<const Statement>>(first)),
    mSecond(std::forward<Ogre::unique_ptr<const Statement>>(second)),
	Statement(line)
{
    if (op == "<") {
        mType = Less;
    } else if (op == ">") {
        mType = Greater;
    } else if (op == "==") {
        mType = Equal;
    } else {
        throw ScriptingException(Database::Exceptions::unknownCompareOperator(op));
    }
}

ValueType Comparator::run(Scope *rootScope, Scope *scope, VarSet &stack, bool *) const
{
    ValueType left = mFirst->run(rootScope, scope, stack);
    ValueType right = mSecond->run(rootScope, 0, stack);
	switch (mType) {
	case Less:
		return left < right;
	case Greater:
		return left > right;
	case Equal:
		return left == right;
	default:
		throw 0;
	}
}

} // namespace Statements
}
} // namespace Scripting

