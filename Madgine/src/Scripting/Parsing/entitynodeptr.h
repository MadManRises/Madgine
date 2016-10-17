#pragma once

#include "entitynode.h"

namespace Engine {
namespace Scripting {
namespace Parsing {
	

class EntityNodePtr : public Ogre::SharedPtr<EntityNode>
{
public:
	EntityNodePtr();
	explicit EntityNodePtr(EntityNode *rep);
	EntityNodePtr(const EntityNodePtr &r);
	EntityNodePtr(const Ogre::ResourcePtr &r);

	/// Operator used to convert a ResourcePtr to a TextFilePtr
	EntityNodePtr& operator=(const Ogre::ResourcePtr& r);
};

}
}
}


