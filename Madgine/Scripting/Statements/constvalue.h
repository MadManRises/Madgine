#pragma once

#include "Scripting/Statements/statement.h"
#include "Scripting/Datatypes/valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class ConstValue : public Statement {
public:
    ConstValue(int line, const ValueType &v);

    virtual ValueType run(Scope *, Scope *, VarSet &, bool *) const;
private:
    ValueType mValue;
};

}
}
}

