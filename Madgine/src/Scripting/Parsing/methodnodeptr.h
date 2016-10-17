#pragma once

#include "methodnode.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class MethodNodePtr : public Ogre::SharedPtr<MethodNode>
{
public:
	MethodNodePtr();
	explicit MethodNodePtr(MethodNode *rep);
	MethodNodePtr(const MethodNodePtr &r);
	MethodNodePtr(const Ogre::ResourcePtr &r);

	/// Operator used to convert a ResourcePtr to a TextFilePtr
	MethodNodePtr& operator=(const Ogre::ResourcePtr& r);
};

}
}
}


