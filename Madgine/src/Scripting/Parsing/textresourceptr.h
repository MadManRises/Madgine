#pragma once

#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class MADGINE_EXPORT TextResourcePtr : public Ogre::SharedPtr<TextResource>
{
public:
	TextResourcePtr();
	explicit TextResourcePtr(TextResource *rep);
	TextResourcePtr(const TextResourcePtr &r);
	TextResourcePtr(const Ogre::ResourcePtr &r);

	/// Operator used to convert a ResourcePtr to a TextFilePtr
	TextResourcePtr& operator=(const Ogre::ResourcePtr& r);
};

}
}
}


