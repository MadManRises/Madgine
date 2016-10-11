#pragma once

#include "Scripting/Statements/statement.h"

namespace Engine {
namespace Scripting {
namespace Statements {

class Comparator : public Statement {
public:
    Comparator(int line, const std::string &op, Ogre::unique_ptr<const Statement> &&first, Ogre::unique_ptr<const Statement> &&second);

    ValueType run(Scope *rootScope, Scope *, VarSet &stack, bool *) const;

private:
    enum {
        Less,
        Greater,
        Equal
    } mType;
    Ogre::unique_ptr<const Statement> mFirst, mSecond;
};

} // namespace Statements
}
} // namespace Scripting


