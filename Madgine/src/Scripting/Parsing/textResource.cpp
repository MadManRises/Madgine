
#include <madginelib.h>
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {



TextResource::TextResource(Ogre::ResourceManager * creator, const Ogre::String & name, Ogre::ResourceHandle handle, const Ogre::String & group, bool isManual, Ogre::ManualResourceLoader * loader) :
	Ogre::Resource(creator, name, handle, group, loader),
	mLineNr(1)
{
	createParamDictionary("Scripting");
}

TextResource::~TextResource()
{
	unload();
}

int TextResource::lineNr()
{
	return mLineNr;
}

void TextResource::setLineNr(int line)
{
	mLineNr = line;
}

}
}
}
