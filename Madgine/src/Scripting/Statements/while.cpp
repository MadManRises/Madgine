#include "madginelib.h"
#include "while.h"
#include "Scripting/Datatypes/valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

While::While(int line, Ogre::unique_ptr<const Statement> &&cond, std::list<Ogre::unique_ptr<const Statement>> &&statements) :
    mCond(std::forward<Ogre::unique_ptr<const Statement>>(cond)),
    mStatements(std::forward<std::list<Ogre::unique_ptr<const Statement>>>(statements)),
	Statement(line)
{

}

ValueType While::run(Scope *rootScope, Scope *scope, VarSet &stack, bool *bReturn) const
{
    while(mCond->run(rootScope, 0, stack).asBool()){
        for (const Ogre::unique_ptr<const Statement> &s : mStatements) {
            ValueType temp = s->run(rootScope, 0, stack, bReturn);
            if (bReturn && *bReturn) return temp;
        }
    }
    return ValueType();
}

} // namespace Statements
} // namespace Scripting
} // namespace Engine

