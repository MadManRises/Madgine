#pragma once


#include "Scripting/Statements/statement.h"


namespace Engine {
namespace Scripting {
namespace Statements {

class If : public Statement {
public:
    If(int line, std::unique_ptr<const Statement> &&cond, std::list<std::unique_ptr<const Statement>> &&statements,
       std::list<std::unique_ptr<const Statement>> &&elseStatements = {});

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack,
                          bool *bReturn) const;

private:
    std::unique_ptr<const Statement> mCond;
    std::list<std::unique_ptr<const Statement>> mStatements;
    std::list<std::unique_ptr<const Statement>> mElseStatements;
};

} // namespace Statements
}
} // namespace Scripting

