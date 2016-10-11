#include "libinclude.h"
#include "entitynode.h"
#include "Scripting/scriptexception.h"
#include "Scripting/Parsing/methodnode.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Parsing {


EntityNode::EntityNode(Ogre::ResourceManager * creator, const Ogre::String & name, Ogre::ResourceHandle handle, const Ogre::String & group, bool isManual, Ogre::ManualResourceLoader * loader) :
	TextResource(creator, name, handle, group, isManual, loader),
	mPrototype(0)
{
	
}

void EntityNode::addMethod(const MethodNodePtr &method, const std::string &name)
{
    auto it = mMethods.find(name);
    if (it != mMethods.end())
        throw ScriptingException(Database::Exceptions::doubleEntityMethod(name, mName));
    mMethods[name] = method;
}

const MethodNodePtr &EntityNode::getMethod(const std::string &name) const
{
    auto it = mMethods.find(name);
    if (it == mMethods.end())
        throw ScriptingException(Database::Exceptions::unknownMethod(name, mName));
    return it->second;
}

bool EntityNode::hasMethod(const std::string &name) const
{
    return mMethods.find(name) != mMethods.end();
}


void EntityNode::setPrototype(Scope * prototype)
{
	mPrototype = prototype;
}

Scope * EntityNode::getPrototype() const
{
	return mPrototype;
}

void EntityNode::unloadImpl()
{
	mName.clear();
	mPrototype = nullptr;
}

void EntityNode::loadImpl()
{
}

size_t EntityNode::calculateSize() const
{
	size_t size = mName.size() + 1;
	for (const std::pair<const std::string, MethodNodePtr> &p : mMethods) {
		size += p.first.size() + 1;
		size += p.second->calculateSize();
	}
	return size;
}


}
}
}
