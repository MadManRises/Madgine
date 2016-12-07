#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class Negate : public Statement {
public:
    Negate(int line, Ogre::unique_ptr<const Statement> &&child);

    virtual ValueType run(Scope *rootScope, Scope *, Stack &stack, bool *) const;
private:
    Ogre::unique_ptr<const Statement> mChild;
};

} // namespace Statements
} // namespace Scripting
}

