#include "madginelib.h"
#include "logicaloperator.h"
#include "Scripting/scriptingexception.h"
#include "valuetype.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {


LogicalOperator::LogicalOperator(int line, const std::string &op, Ogre::unique_ptr<const Statement> &&first,
                                 Ogre::unique_ptr<const Statement> &&second) :
    mFirst(std::forward<Ogre::unique_ptr<const Statement>>(first)),
    mSecond(std::forward<Ogre::unique_ptr<const Statement>>(second)),
	Statement(line)
{
    if (op == "&&") {
        mType = And;
    } else if (op == "||") {
        mType = Or;
    } else {
        throw ScriptingException(Database::Exceptions::unknownLogicalOperator(op));
    }
}

ValueType LogicalOperator::run(Scope *rootScope, Scope *, Stack &stack, bool *) const
{
    bool temp = mFirst->run(rootScope, 0, stack).asBool();
    switch (mType) {
    case And:
        if (!temp) return false;
        break;
    case Or:
        if (temp) return true;
        break;
    }
    return mSecond->run(rootScope, 0, stack).asBool();
}

} // namespace Statements
}
} // namespace Scripting

