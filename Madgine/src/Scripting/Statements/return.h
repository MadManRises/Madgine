#pragma once

#include "Scripting/Statements/statement.h"


namespace Engine {
namespace Scripting {
namespace Statements {

class Return : public Statement {
public:
    Return(int line);
    Return(int line, Ogre::unique_ptr<const Statement> &&value);

    virtual ValueType run(Scope *rootScope, Scope *, Stack &stack, bool *bReturn) const;
private:
    Ogre::unique_ptr<const Statement> mValue;
};

} // namespace Statements
} // namespace Scripting
} // namespace Game

