
#include "entitynodeptr.h"
#include "entitynode.h"

namespace Engine {
namespace Scripting {
namespace Parsing {



	EntityNodePtr::EntityNodePtr() : Ogre::SharedPtr<EntityNode>() {}

	EntityNodePtr::EntityNodePtr(EntityNode * rep) : Ogre::SharedPtr<EntityNode>(rep) {}

	EntityNodePtr::EntityNodePtr(const EntityNodePtr & r) : Ogre::SharedPtr<EntityNode>(r) {}

	EntityNodePtr::EntityNodePtr(const Ogre::ResourcePtr & r) : Ogre::SharedPtr<EntityNode>()
	{
		if (r.isNull())
			return;
		// lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<EntityNode*>(r.getPointer());
		pUseCount = r.useCountPointer();
		useFreeMethod = r.freeMethod();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}

	EntityNodePtr& EntityNodePtr::operator=(const Ogre::ResourcePtr& r)
	{
		if (pRep == static_cast<EntityNode*>(r.getPointer()))
			return *this;
		release();
		if (r.isNull())
			return *this; // resource ptr is null, so the call to release above has done all we need to do.
						  // lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<EntityNode*>(r.getPointer());
		pUseCount = r.useCountPointer();
		useFreeMethod = r.freeMethod();
		if (pUseCount)
		{
			++(*pUseCount);
		}
		return *this;
	}

}
}
}


