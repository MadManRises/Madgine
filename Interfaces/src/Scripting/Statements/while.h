#pragma once

#include "Scripting/Statements/statement.h"


namespace Engine {
namespace Scripting {
namespace Statements {

class While : public Statement
{
public:
    While(int line, std::unique_ptr<const Statement> &&cond, std::list<std::unique_ptr<const Statement>> &&statements);

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack,
                          bool *bReturn) const override;

private:
    std::unique_ptr<const Statement> mCond;
    std::list<std::unique_ptr<const Statement>> mStatements;
};

} // namespace Statements
} // namespace Scripting
} // namespace Engine


