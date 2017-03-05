#include "madginelib.h"
#include "ogrescriptparser.h"

#include "Serialize\OgreStreamReader.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

OgreScriptParser::OgreScriptParser() : mRsgm(
        &Ogre::ResourceGroupManager::getSingleton())
{
    mLoadOrder = 100.0f;
    mScriptPatterns.push_back("*.script");
    mRsgm->_registerScriptLoader(this);
}

OgreScriptParser::~OgreScriptParser()
{

	if (mRsgm->getSingletonPtr()) {
		mRsgm->_unregisterScriptLoader(this);
	}
}


const Ogre::StringVector &OgreScriptParser::getScriptPatterns() const
{
    return mScriptPatterns;
}

float OgreScriptParser::getLoadingOrder() const
{
    return mLoadOrder;
}

void OgreScriptParser::parseScript(Ogre::DataStreamPtr &stream,
	const Ogre::String &group)
{
	Serialize::OgreStreamReader reader(stream);
	std::istream is(&reader);
	ScriptParser::parseScript(is, group, false);
}


}
}
}
