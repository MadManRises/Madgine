
#include "methodnodeptr.h"

namespace Engine {
namespace Scripting {
namespace Parsing {
	MethodNodePtr::MethodNodePtr() : Ogre::SharedPtr<MethodNode>() {}
	MethodNodePtr::MethodNodePtr(MethodNode * rep) : Ogre::SharedPtr<MethodNode>(rep) {}
	MethodNodePtr::MethodNodePtr(const MethodNodePtr & r) : Ogre::SharedPtr<MethodNode>(r) {}
	MethodNodePtr::MethodNodePtr(const Ogre::ResourcePtr & r) : Ogre::SharedPtr<MethodNode>()
	{
		if (r.isNull())
			return;
		// lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<MethodNode*>(r.getPointer());
		pUseCount = r.useCountPointer();
		useFreeMethod = r.freeMethod();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}

	MethodNodePtr& MethodNodePtr::operator=(const Ogre::ResourcePtr& r)
	{
		if (pRep == static_cast<MethodNode*>(r.getPointer()))
			return *this;
		release();
		if (r.isNull())
			return *this; // resource ptr is null, so the call to release above has done all we need to do.
						  // lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<MethodNode*>(r.getPointer());
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


