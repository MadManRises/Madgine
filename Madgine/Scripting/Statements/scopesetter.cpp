#include "libinclude.h"
#include "scopesetter.h"
#include "Scripting\Datatypes\valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

ScopeSetter::ScopeSetter(int line, Ogre::unique_ptr<const Statement> &&child, Ogre::unique_ptr<const Statement> &&scope) :
    mChild(std::forward<Ogre::unique_ptr<const Statement>>(child)),
    mScope(std::forward<Ogre::unique_ptr<const Statement>>(scope)),
	Statement(line)
{

}

ValueType ScopeSetter::run(Scope *rootScope, Scope *scope, VarSet &stack,
                           bool *) const
{
    Scope *newScope = mScope->run(rootScope, scope, stack).asScope();
    return mChild->run(rootScope, newScope, stack);
}

}
}
}
