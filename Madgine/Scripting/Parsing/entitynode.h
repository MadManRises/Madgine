#pragma once

#include "methodnode.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class EntityNode : public TextResource {
public:
    EntityNode(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);
	EntityNode(const EntityNode &) = delete;

    void addMethod(const MethodNodePtr &method, const std::string &name);
    const MethodNodePtr &getMethod(const std::string &name) const;
    bool hasMethod(const std::string &name) const;
	void setPrototype(Scope *prototype);
	Scope *getPrototype() const;

protected:
	virtual void unloadImpl() override final;
	virtual void loadImpl() override final;
	virtual size_t calculateSize() const override final;

private:
    std::map<std::string, MethodNodePtr> mMethods;
	Scope *mPrototype;
};


class EntityNodePtr : public Ogre::SharedPtr<EntityNode>
{
public:
	EntityNodePtr() : Ogre::SharedPtr<EntityNode>() {}
	explicit EntityNodePtr(EntityNode *rep) : Ogre::SharedPtr<EntityNode>(rep) {}
	EntityNodePtr(const EntityNodePtr &r) : Ogre::SharedPtr<EntityNode>(r) {}
	EntityNodePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<EntityNode>()
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

	/// Operator used to convert a ResourcePtr to a TextFilePtr
	EntityNodePtr& operator=(const Ogre::ResourcePtr& r)
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
};

}
}
}


