#include "interfaceslib.h"
#include "while.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

While::While(int line, std::unique_ptr<const Statement> &&cond, std::list<std::unique_ptr<const Statement>> &&statements) :
    mCond(std::forward<std::unique_ptr<const Statement>>(cond)),
    mStatements(std::forward<std::list<std::unique_ptr<const Statement>>>(statements)),
	Statement(line)
{

}

ValueType While::run(Scope *rootScope, Scope *scope, Stack &stack, bool *bReturn) const
{
    while(mCond->run(rootScope, 0, stack).asBool()){
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

