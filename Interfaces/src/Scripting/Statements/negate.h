#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class Negate : public Statement {
public:
    Negate(int line, std::unique_ptr<const Statement> &&child);

    virtual ValueType run(Scope *rootScope, Scope *, Stack &stack, bool *) const;
private:
    std::unique_ptr<const Statement> mChild;
};

} // namespace Statements
} // namespace Scripting
}

