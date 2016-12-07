#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {


class MethodCall : public Statement {
public:
    MethodCall(int line, const std::string &methodName,
               std::list<Ogre::unique_ptr<const Statement> > &&arguments);

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack, bool *) const;

private:
    std::string mMethodName;
    std::list<Ogre::unique_ptr<const Statement>> mArguments;
};

}
}
}


