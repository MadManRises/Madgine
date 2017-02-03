#include "interfaceslib.h"
#include "negate.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

Negate::Negate(int line, std::unique_ptr<const Statement> &&child) :
    mChild(std::forward<std::unique_ptr<const Statement>>(child)),
	Statement(line)
{

}

ValueType Negate::run(Scope *rootScope, Scope *, Stack &stack, bool *) const
{
    return ValueType(!mChild->run(rootScope, 0, stack).asBool());
}

} // namespace Statements
} // namespace Scripting
}

