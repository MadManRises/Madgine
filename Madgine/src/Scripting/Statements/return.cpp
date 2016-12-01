#include "madginelib.h"
#include "return.h"

#include "Scripting/Datatypes/valuetype.h"
#include "Scripting/scriptingexception.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {

Return::Return(int line) :
	Statement(line)
{

}

Return::Return(int line, Ogre::unique_ptr<const Statement> &&value) :
    mValue(std::forward<Ogre::unique_ptr<const Statement>>(value)),
	Statement(line)
{

}

ValueType Return::run(Scope *rootScope, Scope *, VarSet &stack, bool *bReturn) const
{
    if (!bReturn)
        throw ScriptingException(Database::Exceptions::invalidReturn);
    *bReturn = true;
    return mValue ? mValue->run(rootScope, 0, stack) : ValueType();
}

} // namespace Statements
} // namespace Scripting
}

