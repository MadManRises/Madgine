#include "../../clientlib.h"
#include "ogrescriptparser.h"

#include "../../serialize/ogrestreamreader.h"

#include "../types/luastate.h"

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			OgreScriptParser::OgreScriptParser(LuaState* state) :
				ScriptParser(state),
				mRsgm(&Ogre::ResourceGroupManager::getSingleton())
			{
				mLoadOrder = 100.0f;
				mScriptPatterns.push_back(std::string("*") + fileExtension());
				mRsgm->_registerScriptLoader(this);
			}

			OgreScriptParser::~OgreScriptParser()
			{
				if (mRsgm->getSingletonPtr())
				{
					mRsgm->_unregisterScriptLoader(this);
				}
			}


			const Ogre::StringVector& OgreScriptParser::getScriptPatterns() const
			{
				return mScriptPatterns;
			}

			float OgreScriptParser::getLoadingOrder() const
			{
				return mLoadOrder;
			}

			void OgreScriptParser::parseScript(Ogre::DataStreamPtr& stream,
			                                   const Ogre::String& group)
			{
				Serialize::OgreStreamReader reader(Ogre::dynamic_pointer_cast<Ogre::MemoryDataStream>(stream));
				std::istream is(&reader);
				ScriptParser::parseScript(is, group, false);
			}

			void OgreScriptParser::setFinalized()
			{
				state()->setFinalized();
			}
		}
	}
}
