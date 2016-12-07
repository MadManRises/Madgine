#pragma once

namespace Engine {
namespace Scripting {
namespace Statements {

class Statement : public Ogre::ScriptingAllocatedObject {

public:
    Statement(int line) :
		mLineNr(line)
    {
    }

    virtual ~Statement()
    {
    }

	int getLine() const {
		return mLineNr;
	}

    virtual ValueType run(Scope *rootScope, Scope *scope, Stack &stack,
                          bool *bReturn = 0) const = 0;

private:
	const int mLineNr;

};

}
}
}

