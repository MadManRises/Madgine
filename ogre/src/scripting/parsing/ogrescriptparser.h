#pragma once

#include "scripting/parsing/scriptparser.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class OgreScriptParser : public Ogre::ScriptLoader, public Ogre::GeneralAllocatedObject,
    public ScriptParser {
public:
    OgreScriptParser(LuaState *state);
	OgreScriptParser(const OgreScriptParser &) = delete;
    virtual ~OgreScriptParser();

	void operator=(const OgreScriptParser &) = delete;

    virtual const Ogre::StringVector &getScriptPatterns() const override;
    virtual float getLoadingOrder() const override;

    virtual void parseScript(Ogre::DataStreamPtr &stream, const Ogre::String &group) override;

	void setFinalized();

protected:
	

private:

    float mLoadOrder;
    Ogre::StringVector mScriptPatterns;

    Ogre::ResourceGroupManager *mRsgm;


};

}
}
}


