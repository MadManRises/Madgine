#pragma once

#include "Scripting/Statements/statement.h"


namespace Engine {
namespace Scripting {
namespace Statements {

class Accessor : public Statement {
public:
    Accessor(int line, const std::string &attName);

    ValueType run(Scope *rootScope, Scope *scope, VarSet &stack, bool *) const;

private:
    std::string mAttName;
};

}
}
}

