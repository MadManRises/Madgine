#include "interfaceslib.h"
#include "return.h"

#include "valuetype.h"
#include "Scripting/scriptingexception.h"

#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Statements {

Return::Return(int line) :
	Statement(line)
{

}

Return::Return(int line, std::unique_ptr<const Statement> &&value) :
    mValue(std::forward<std::unique_ptr<const Statement>>(value)),
	Statement(line)
{

}

ValueType Return::run(Scope *rootScope, Scope *, Stack &stack, bool *bReturn) const
{
    if (!bReturn)
        throw ScriptingException(Exceptions::invalidReturn);
    *bReturn = true;
    return mValue ? mValue->run(rootScope, 0, stack) : ValueType();
}

} // namespace Statements
} // namespace Scripting
}

