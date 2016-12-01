#include "madginelib.h"
#include "for.h"
#include "Scripting/Datatypes/valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {


For::For(int line, Ogre::unique_ptr<const Statement> &&init, Ogre::unique_ptr<const Statement> &&cond, Ogre::unique_ptr<const Statement> &&step, std::list<Ogre::unique_ptr<const Statement> > &&statements) :
    mInit(std::forward<Ogre::unique_ptr<const Statement>>(init)),
    mCond(std::forward<Ogre::unique_ptr<const Statement>>(cond)),
    mStep(std::forward<Ogre::unique_ptr<const Statement>>(step)),
    mStatements(std::forward<std::list<Ogre::unique_ptr<const Statement>>>(statements)),
	Statement(line)
{

}

ValueType For::run(Scope *rootScope, Scope *scope, VarSet &stack, bool *bReturn) const
{

    for(mInit->run(rootScope, 0, stack); mCond->run(rootScope, 0, stack).asBool(); mStep->run(rootScope, 0, stack)){
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

