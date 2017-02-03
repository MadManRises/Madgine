#pragma once

#include "statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class LogicalOperator : public Statement {
public:
    LogicalOperator(int line, const std::string &op, std::unique_ptr<const Statement> &&first, std::unique_ptr<const Statement> &&second);

    ValueType run(Scope *rootScope, Scope *, Stack &stack, bool *) const;

private:
    enum {
        Or,
        And
    } mType;
    std::unique_ptr<const Statement> mFirst, mSecond;
};

} // namespace Statements
}
} // namespace Scripting

