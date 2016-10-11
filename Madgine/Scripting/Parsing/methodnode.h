#pragma once

#include "Scripting/Statements/statement.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class MethodNode : public TextResource {
public:
    MethodNode(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);
	MethodNode(const MethodNode &) = delete;

    ValueType run(Scope *scope, VarSet &stack) const;
    void addArgument(const std::string &arg);
    void addStatement(Ogre::unique_ptr<const Statements::Statement> &&s);

    const std::list<std::string> &arguments() const;

	virtual size_t calculateSize() const override final;
protected:
	virtual void loadImpl() override final;
	virtual void unloadImpl() override final;

private:
    std::list<Ogre::unique_ptr<const Statements::Statement>> mStatements;
    std::list<std::string> mArguments;
};

class MethodNodePtr : public Ogre::SharedPtr<MethodNode>
{
public:
	MethodNodePtr() : Ogre::SharedPtr<MethodNode>() {}
	explicit MethodNodePtr(MethodNode *rep) : Ogre::SharedPtr<MethodNode>(rep) {}
	MethodNodePtr(const MethodNodePtr &r) : Ogre::SharedPtr<MethodNode>(r) {}
	MethodNodePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<MethodNode>()
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

	/// Operator used to convert a ResourcePtr to a TextFilePtr
	MethodNodePtr& operator=(const Ogre::ResourcePtr& r)
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
};

}
}
}


