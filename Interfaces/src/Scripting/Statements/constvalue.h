#pragma once

#include "Scripting/Statements/statement.h"
#include "valuetype.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class ConstValue : public Statement {
public:
    ConstValue(int line, const ValueType &v);

    virtual ValueType run(Scope *, Scope *, Stack &, bool *) const;
private:
    ValueType mValue;
};

}
}
}

