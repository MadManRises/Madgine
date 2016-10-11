#include "libinclude.h"
#include "methodnode.h"
#include "Scripting/Datatypes/valuetype.h"
#include "Scripting/Statements/statement.h"
#include "Scripting/scriptexception.h"
#include "Scripting\Types\scope.h"
#include "Util\UtilMethods.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

MethodNode::MethodNode(Ogre::ResourceManager *creator, const Ogre::String &name,
	Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual,
	Ogre::ManualResourceLoader *loader) :
	TextResource(creator, name, handle, group, isManual, loader)
{

}


ValueType MethodNode::run(Scope *scope, VarSet &stack) const
{
    bool bReturn = false;
	for (const Ogre::unique_ptr<const Statements::Statement> &s : mStatements) {
		TRACE_S(getOrigin(), s->getLine(), getName());
		ValueType temp = s->run(scope, 0, stack, &bReturn);
		if (bReturn)
			return temp;
	}
    return ValueType();
}

void MethodNode::addArgument(const std::string &arg)
{
    mArguments.push_back(arg);
}

void MethodNode::addStatement(Ogre::unique_ptr<const Statements::Statement> &&s)
{
    mStatements.emplace_back(std::forward<Ogre::unique_ptr<const Statements::Statement>>(s));
}

const std::list<std::string> &MethodNode::arguments() const
{
    return mArguments;
}

void MethodNode::loadImpl()
{
}

void MethodNode::unloadImpl()
{
}

size_t MethodNode::calculateSize() const
{
	size_t size = 4 * mStatements.size();
	for (const std::string &arg : mArguments)
		size += arg.size() + 1;

	return size;
}

}
}
}
