#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class Comparator : public Statement {
public:
    Comparator(int line, const std::string &op, std::unique_ptr<const Statement> &&first, std::unique_ptr<const Statement> &&second);

    ValueType run(Scope *rootScope, Scope *, Stack &stack, bool *) const;

private:
    enum {
        Less,
        Greater,
        Equal,
		Unequal
    } mType;
    std::unique_ptr<const Statement> mFirst, mSecond;
};

} // namespace Statements
}
} // namespace Scripting


