#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {


class ScopeSetter : public Statement {
public:
    ScopeSetter(int line, Ogre::unique_ptr<const Statement> &&child, Ogre::unique_ptr<const Statement> &&scope);

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack, bool *) const;

private:
    Ogre::unique_ptr<const Statement> mChild, mScope;
};

}
}
}

