#include "libinclude.h"
#include "scope.h"
#include "Scripting/Datatypes/Serialize/serializestream.h"
#include "Scripting/scriptingexception.h"
#include "Scripting/Parsing/methodnodePtr.h"
#include "Scripting/Parsing/methodnode.h"
#include "Scripting/Datatypes/Serialize/serializeexception.h"

#include "Database/exceptionmessages.h"
#include "Scripting\Parsing\scriptparser.h"

#include "Util\UtilMethods.h"

namespace Engine {
namespace Scripting {

const std::map<std::string, Scope::NativeMethod> Scope::mBoundNativeMethods = {
    {"hasProperty", &hasProperty},
    {"set", &set},
    {"get", &get},
    {"check", &checkFlag},
	{"findPrototype", &findPrototype},
	{"getPrototype", &getPrototype},
	{"setPrototype", &setPrototype}
};

Scope::Scope(const std::string &prototypeName) :
	mPrototype(0)
{
	if (!prototypeName.empty())
		findPrototype(prototypeName);
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
		return mVariables.get(name);
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

void Scope::save(Serialize::SerializeOutStream &of) const
{
    of << mVariables;
}

void Scope::load(Serialize::SerializeInStream &ifs)
{
    clear();
    ifs >> mVariables;
}

void Scope::applyScopeMap(const std::map<InvScopePtr, Scope *> &map)
{
    std::list<ValueType *> values;
    collectValueRefs(values);
    for (ValueType *v : values){
        if (v->isInvScope()){
            auto it = map.find(v->asInvScope());
            if (it == map.end()) throw 0;
            *v = it->second;
        }
    }
}

bool Scope::checkFlag(const std::string &name, bool defaultValue)
{
    return hasVar(name) ? mVariables[name].asBool() : defaultValue;
}

void Scope::storeCreationData(Serialize::SerializeOutStream &of)
{
    of << (int)getClassType();
}

void Scope::collectScopes(std::set<Scope *> &scopeMap, const std::set<Scope *> &ignoreMap)
{
    if (ignoreMap.find(this) != ignoreMap.end()) return;
    if (scopeMap.find(this) != scopeMap.end()) return;

    scopeMap.insert(this);
    std::list<ValueType *> values;
    collectValueRefs(values);
    for (ValueType *v : values){
        if (v->isScope()){
            v->asScope()->collectScopes(scopeMap);
        }
    }
    return;
}

void Scope::collectNamedValues(std::map<std::string, ValueType *> &values)
{
    for (std::pair<const std::string, ValueType> &p : mVariables){
        values[p.first] = &p.second;
    }

}

void Scope::clear()
{
    mVariables.clear();
}

void Scope::collectValueRefs(std::list<ValueType *> &values)
{
    for (std::pair<const std::string, ValueType> &p : mVariables){
        values.push_back(&p.second);
    }
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
    return mVariables.get(stack.at(0).asString());
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

ValueType Scope::execScriptMethod(const std::string &name,
                                  const ArgumentList &args)
{
    if (!hasScriptMethod(name))
        MADGINE_THROW_NO_TRACE(ScriptingException(
            Database::Exceptions::unknownMethod(name, getIdentifier())));
    if (getArguments(name).size() != args.size()) 
		MADGINE_THROW_NO_TRACE(ScriptingException(
            Database::Exceptions::argumentCountMismatch(getArguments(name).size(), args.size())));
    VarSet stack;

    auto it = getArguments(name).begin();
    for (const ValueType &v : args) {
        stack[*it] = v;
        ++it;
    }
    return getMethod(name)->run(this, stack);
}

}
}
