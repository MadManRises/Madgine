#pragma once


#include "Scripting/Statements/statement.h"


namespace Engine {
namespace Scripting {
namespace Statements {

class If : public Statement {
public:
    If(int line, Ogre::unique_ptr<const Statement> &&cond, std::list<Ogre::unique_ptr<const Statement>> &&statements,
       std::list<Ogre::unique_ptr<const Statement>> &&elseStatements = {});

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack,
                          bool *bReturn) const;

private:
    Ogre::unique_ptr<const Statement> mCond;
    std::list<Ogre::unique_ptr<const Statement>> mStatements;
    std::list<Ogre::unique_ptr<const Statement>> mElseStatements;
};

} // namespace Statements
}
} // namespace Scripting

