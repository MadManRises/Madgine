#include "libinclude.h"
#include "negate.h"
#include "Scripting/Datatypes/valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

Negate::Negate(int line, Ogre::unique_ptr<const Statement> &&child) :
    mChild(std::forward<Ogre::unique_ptr<const Statement>>(child)),
	Statement(line)
{

}

ValueType Negate::run(Scope *rootScope, Scope *, VarSet &stack, bool *) const
{
    return ValueType(!mChild->run(rootScope, 0, stack).asBool());
}

} // namespace Statements
} // namespace Scripting
}

