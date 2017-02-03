#include "interfaceslib.h"
#include "if.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

If::If(int line, std::unique_ptr<const Statement> &&cond, std::list<std::unique_ptr<const Statement>> &&statements,
       std::list<std::unique_ptr<const Statement>> &&elseStatements) :
    mCond(std::forward<std::unique_ptr<const Statement>>(cond)),
    mStatements(std::forward<std::list<std::unique_ptr<const Statement>>>(statements)),
    mElseStatements(std::forward<std::list<std::unique_ptr<const Statement>>>(elseStatements)),
	Statement(line)
{

}

ValueType If::run(Scope *rootScope, Scope *scope, Stack &stack, bool *bReturn) const
{
    const std::list<std::unique_ptr<const Statement>> &statements = (mCond->run(rootScope, 0,
                                          stack).asBool() ? mStatements : mElseStatements);
    for (const std::unique_ptr<const Statement> &s : statements) {
        ValueType temp = s->run(rootScope, 0, stack, bReturn);
        if (bReturn && *bReturn) return temp;
    }
    return ValueType();
}

} // namespace Statements
}
} // namespace Scripting

