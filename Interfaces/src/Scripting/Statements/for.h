#pragma once


#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class For : public Statement
{
public:
    For(int line, std::unique_ptr<const Statement> &&init, std::unique_ptr<const Statement> &&cond, std::unique_ptr<const Statement> &&step, std::list<std::unique_ptr<const Statement>> &&statements);

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack,
                          bool *bReturn) const override;

private:
    std::unique_ptr<const Statement> mInit, mCond, mStep;
    std::list<std::unique_ptr<const Statement>> mStatements;
};

} // namespace Statements
} // namespace Scripting
} // namespace Engine

