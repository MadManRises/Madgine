#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {


class Assignment : public Statement {
public:
    Assignment(int line, const std::string &varName, const std::string &op, std::unique_ptr<const Statement> &&value);

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack, bool *) const;

private:
	enum {
		ASSIGNMENT,
		APPEND,
		SUBTRACT
	} mOp;

    std::string mVarName;
    std::unique_ptr<const Statement> mValue;
};

}
}
}

