#include "libinclude.h"
#include "accessor.h"
#include "Scripting/Types/scope.h"
#include "Scripting/Datatypes/varset.h"
#include "Scripting/Types/globalscope.h"

namespace Engine {
namespace Scripting {
namespace Statements {


Accessor::Accessor(int line, const std::string &attName) :
	Statement(line),
	mAttName(attName)	
{

}

ValueType Accessor::run(Scope *rootScope, Scope *scope, VarSet &stack, bool *) const
{
    if (mAttName.empty())
        return &GlobalScope::getSingleton();
    else if (mAttName == "my")
        return rootScope;
    else if (scope)
        return scope->getVar(mAttName);
    else
        return stack.get(mAttName);
}

}
}
}
