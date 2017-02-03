#include "interfaceslib.h"
#include "scopesetter.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

ScopeSetter::ScopeSetter(int line, std::unique_ptr<const Statement> &&child, std::unique_ptr<const Statement> &&scope) :
    mChild(std::forward<std::unique_ptr<const Statement>>(child)),
    mScope(std::forward<std::unique_ptr<const Statement>>(scope)),
	Statement(line)
{

}

ValueType ScopeSetter::run(Scope *rootScope, Scope *scope, Stack &stack,
                           bool *) const
{
    Scope *newScope = mScope->run(rootScope, scope, stack).asScope();
    return mChild->run(rootScope, newScope, stack);
}

}
}
}
