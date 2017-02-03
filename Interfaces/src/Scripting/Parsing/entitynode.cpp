#include "interfaceslib.h"
#include "entitynode.h"
#include "Scripting/scriptingexception.h"
#include "Scripting/Parsing/methodnode.h"

#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {
namespace Parsing {


MethodNode &EntityNode::addMethod(const std::string &name, const std::string &origin)
{
    auto it = mMethods.find(name);
    if (it != mMethods.end())
        throw ScriptingException(Exceptions::doubleEntityMethod(name, mName));
	return mMethods.try_emplace(name, name, origin).first->second;
}

void EntityNode::removeMethod(const std::string & name)
{
	mMethods.erase(name);
}

const std::map<std::string, MethodNode>& EntityNode::getMethods() const
{
	return mMethods;
}

const MethodNode &EntityNode::getMethod(const std::string &name) const
{
    auto it = mMethods.find(name);
    if (it == mMethods.end())
        throw ScriptingException(Exceptions::unknownMethod(name, mName));
    return it->second;
}

bool EntityNode::hasMethod(const std::string &name) const
{
    return mMethods.find(name) != mMethods.end();
}


void EntityNode::setPrototype(const std::string &prototype)
{
	mPrototype = prototype;
}

const std::string &EntityNode::getPrototype() const
{
	return mPrototype;
}

void EntityNode::clear()
{
	mPrototype.clear();
	mMethods.clear();
}


}
}
}
