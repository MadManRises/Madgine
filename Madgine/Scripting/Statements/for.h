#pragma once


#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class For : public Statement
{
public:
    For(int line, Ogre::unique_ptr<const Statement> &&init, Ogre::unique_ptr<const Statement> &&cond, Ogre::unique_ptr<const Statement> &&step, std::list<Ogre::unique_ptr<const Statement>> &&statements);

    virtual ValueType run(Scope *rootScope, Scope *scope, VarSet &stack,
                          bool *bReturn) const override;

private:
    Ogre::unique_ptr<const Statement> mInit, mCond, mStep;
    std::list<Ogre::unique_ptr<const Statement>> mStatements;
};

} // namespace Statements
} // namespace Scripting
} // namespace Engine

