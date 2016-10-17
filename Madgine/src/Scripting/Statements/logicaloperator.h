#pragma once

#include "statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class LogicalOperator : public Statement {
public:
    LogicalOperator(int line, const std::string &op, Ogre::unique_ptr<const Statement> &&first, Ogre::unique_ptr<const Statement> &&second);

    ValueType run(Scope *rootScope, Scope *, VarSet &stack, bool *) const;

private:
    enum {
        Or,
        And
    } mType;
    Ogre::unique_ptr<const Statement> mFirst, mSecond;
};

} // namespace Statements
}
} // namespace Scripting

