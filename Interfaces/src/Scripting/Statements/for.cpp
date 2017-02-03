#include "interfaceslib.h"
#include "for.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {


For::For(int line, std::unique_ptr<const Statement> &&init, std::unique_ptr<const Statement> &&cond, std::unique_ptr<const Statement> &&step, std::list<std::unique_ptr<const Statement> > &&statements) :
    mInit(std::forward<std::unique_ptr<const Statement>>(init)),
    mCond(std::forward<std::unique_ptr<const Statement>>(cond)),
    mStep(std::forward<std::unique_ptr<const Statement>>(step)),
    mStatements(std::forward<std::list<std::unique_ptr<const Statement>>>(statements)),
	Statement(line)
{

}

ValueType For::run(Scope *rootScope, Scope *scope, Stack &stack, bool *bReturn) const
{

    for(mInit->run(rootScope, 0, stack); mCond->run(rootScope, 0, stack).asBool(); mStep->run(rootScope, 0, stack)){
        for (const std::unique_ptr<const Statement> &s : mStatements) {
            ValueType temp = s->run(rootScope, 0, stack, bReturn);
            if (bReturn && *bReturn) return temp;
        }
    }
    return ValueType();
}

} // namespace Statements
} // namespace Scripting
} // namespace Engine

