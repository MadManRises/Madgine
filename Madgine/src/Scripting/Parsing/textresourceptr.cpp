
#include "textresourceptr.h"

namespace Engine {
namespace Scripting {
namespace Parsing {
	TextResourcePtr::TextResourcePtr() : Ogre::SharedPtr<TextResource>() {}
	TextResourcePtr::TextResourcePtr(TextResource * rep) : Ogre::SharedPtr<TextResource>(rep) {}
	TextResourcePtr::TextResourcePtr(const TextResourcePtr & r) : Ogre::SharedPtr<TextResource>(r) {}
	TextResourcePtr::TextResourcePtr(const Ogre::ResourcePtr & r) : Ogre::SharedPtr<TextResource>()
	{
		if (r.isNull())
			return;
		// lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<TextResource*>(r.getPointer());
		pUseCount = r.useCountPointer();
		useFreeMethod = r.freeMethod();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}

	TextResourcePtr& TextResourcePtr::operator=(const Ogre::ResourcePtr& r)
	{
		if (pRep == static_cast<TextResource*>(r.getPointer()))
			return *this;
		release();
		if (r.isNull())
			return *this; // resource ptr is null, so the call to release above has done all we need to do.
						  // lock & copy other mutex pointer
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<TextResource*>(r.getPointer());
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


