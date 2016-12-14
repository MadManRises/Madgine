#include "madginelib.h"
#include "scope.h"
#include "Scripting/scriptingexception.h"
#include "Scripting/Parsing/methodnodePtr.h"
#include "Scripting/Parsing/methodnode.h"

#include "Database/exceptionmessages.h"
#include "Scripting\Parsing\scriptparser.h"

#include "Util\UtilMethods.h"

#include "scriptingmanager.h"

namespace Engine {
namespace Scripting {

const std::map<std::string, Scope::NativeMethod> Scope::mBoundNativeMethods = {
    {"hasProperty", &hasProperty},
    {"set", &set},
    {"get", &get},
    {"check", &checkFlag},
	{"findPrototype", &findPrototype},
	{"getPrototype", &getPrototype},
	{"setPrototype", &setPrototype},
	{"call", &call},
	{"isGlobal", &isGlobal}
};

Scope::Scope(Serialize::TopLevelSerializableUnit *topLevel) :
	mPrototype(0),
	mPrototypeName(this),
	mVariables(this),
	SerializableUnit(topLevel)
{

}

Scope::~Scope()
{
    clear();
}

void Scope::setVar(const std::string &name, const ValueType &v)
{
    mVariables[name] = v;
}

const ValueType &Scope::getVar(const std::string &name) const
{
	if (mPrototype == nullptr || hasVar(name))
		return mVariables.at(name);
	return mPrototype->getVar(name);
}

ValueType &Scope::accessVar(const std::string &name)
{
    return mVariables[name];
}

bool Scope::hasVar(const std::string &name) const
{
    return mVariables.contains(name);
}

void Scope::findPrototype(const std::string & data)
{
	mPrototype = &Parsing::ScriptParser::getSingleton().getPrototype(data);
	mPrototypeName = data;
}

void Scope::setPrototype(Scope * prototype)
{
	mPrototypeName.clear();
	mPrototype = prototype;
}


const std::string &Scope::getPrototype() {
	return mPrototypeName;
}

void Scope::clearPrototype()
{
	mPrototype = 0;
	mPrototypeName.clear();
}

ValueType Scope::methodCall(const std::string &name, const ArgumentList &args)
{
    ValueType result;
    if (mBoundNativeMethods.find(name) != mBoundNativeMethods.end()) {
        auto it = mBoundNativeMethods.find(name);
        result = (this->*(it->second))(args);
    } else {
        result = execScriptMethod(name, args);
    }
    return result;
}

bool Scope::callMethodIfAvailable(const std::string & name, const ArgumentList & args)
{
	if (hasScriptMethod(name))
		return callMethodCatch(name, args);
	else
		return false;
}

bool Scope::callMethodCatch(const std::string & name, const ArgumentList & args)
{
	try {
		methodCall(name, args);
		return true;
	}
	catch (std::exception &e) {
		LOG_EXCEPTION(e);
	}
	return false;
}


void Scope::readState(Serialize::SerializeInStream &in)
{
	SerializableUnit::readState(in);

	if (mPrototypeName.empty())
		mPrototype = 0;
	else
		mPrototype = &Parsing::ScriptParser::getSingleton().getPrototype(mPrototypeName);
}

bool Scope::checkFlag(const std::string &name, bool defaultValue)
{
    return hasVar(name) ? mVariables[name].asBool() : defaultValue;
}

void Scope::clear()
{
    mVariables.clear();
}

const std::list<std::string> &Scope::getArguments(const std::string &name)
{
    return getMethod(name)->arguments();
}

const Parsing::MethodNodePtr &Scope::getMethod(const std::string &name)
{
    throw ScriptingException(Database::Exceptions::noMethods(getIdentifier()));
}

bool Scope::hasScriptMethod(const std::string &name)
{
    return false;
}


ValueType Scope::hasProperty(const ArgumentList &stack)
{
    return mVariables.contains(stack.at(0).asString());
}

ValueType Scope::set(const ArgumentList &stack)
{
    mVariables[stack.at(0).asString()] = stack.at(1);
    return stack.at(1);
}

ValueType Scope::get(const ArgumentList &stack)
{
    return mVariables.at(stack.at(0).asString());
}

ValueType Scope::checkFlag(const ArgumentList &stack)
{
    return stack.size() > 1 ? checkFlag(stack.at(0).asString(), stack.at(1).asBool()) : checkFlag(stack.at(0).asString());
}

ValueType Scope::findPrototype(const ArgumentList & stack)
{
	findPrototype(stack.at(0).asString());
	return ValueType();
}

ValueType Scope::getPrototype(const ArgumentList & stack)
{
	return getPrototype();
}

ValueType Scope::setPrototype(const ArgumentList & stack)
{
	setPrototype(stack.at(0).asScope());
	return ValueType();
}

ValueType Scope::call(const ArgumentList & stack)
{
	if (stack.size() < 1)
		throw ScriptingException("Too few Arguments to call!");
	auto it = stack.begin();
	++it;
	return methodCall(stack.at(0).asString(), { it, stack.end() });
}

ValueType Scope::isGlobal(const ArgumentList & stack)
{
	return topLevel() == &ScriptingManager::getSingleton();
}

ValueType Scope::execScriptMethod(const std::string &name,
                                  const ArgumentList &args)
{
    if (!hasScriptMethod(name))
        MADGINE_THROW_NO_TRACE(ScriptingException(
            Database::Exceptions::unknownMethod(name, getIdentifier())));
    if (getArguments(name).size() != args.size()) 
		MADGINE_THROW_NO_TRACE(ScriptingException(
            Database::Exceptions::argumentCountMismatch(getArguments(name).size(), args.size())));
    Stack stack;

    auto it = getArguments(name).begin();
    for (const ValueType &v : args) {
        stack[*it] = v;
        ++it;
    }
    return getMethod(name)->run(this, stack);
}

}
}
