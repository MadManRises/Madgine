#include "libinclude.h"
#include "scope.h"
#include "Scripting/Datatypes/Serialize/serializestream.h"
#include "Scripting/scriptexception.h"
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
	{"findData", &findData},
	{"setData", &setData}
};

Scope::Scope(Scope *data) :
mPrototype(data)
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

void Scope::findData(const std::string & data)
{
	mPrototype = &Parsing::ScriptParser::getSingleton().getPrototype(data);
}

void Scope::setData(Scope * data)
{
	mPrototype = data;
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

ValueType Scope::findData(const ArgumentList & stack)
{
	findData(stack.at(0).asString());
	return ValueType();
}

ValueType Scope::setData(const ArgumentList & stack)
{
	setData(stack.at(0).asScope());
	return ValueType();
}

ValueType Scope::execScriptMethod(const std::string &name,
                                  const ArgumentList &args)
{
    if (!hasScriptMethod(name))
        MADGINE_THROW(ScriptingException(
            Database::Exceptions::unknownMethod(name, getIdentifier())));
    if (getArguments(name).size() != args.size()) throw ScriptingException(
            Database::Exceptions::argumentCountMismatch(getArguments(name).size(), args.size()));
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
