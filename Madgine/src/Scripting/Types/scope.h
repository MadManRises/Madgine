#pragma once

#include "Serialize\Container\observed.h"
#include "Serialize\serializableunit.h"
#include "Serialize\Container\map.h"

namespace Engine {
namespace Scripting {


class MADGINE_EXPORT Scope : public Ogre::ScriptingAllocatedObject, public Serialize::SerializableUnit {

public:
	Scope(const std::string &prototypeName = "");
    virtual ~Scope();

    void setVar(const std::string &name, const ValueType &v);
    const ValueType &getVar(const std::string &name) const;
    ValueType &accessVar(const std::string &name);
    bool hasVar(const std::string &name) const;
	bool checkFlag(const std::string &name, bool defaultValue = false);
    
	void findPrototype(const std::string &name);
	void setPrototype(Scope *prototype);
	const std::string &getPrototype();
	void clearPrototype();

	virtual bool hasScriptMethod(const std::string &name);
	
	void applyScopeMap(const std::map<InvPtr, Scope *> &map);
	void collectScopes(std::set<Scope *> &scopeMap, const std::set<Scope *> &ignoreMap = {});
	//virtual void collectNamedValues(std::map<std::string, ValueType*> &values);

 
	virtual std::string getIdentifier() = 0;	

	virtual ValueType methodCall(const std::string &name,
		const ArgumentList &args = {});

	bool callMethodIfAvailable(const std::string &name, const ArgumentList &args = {});
	bool callMethodCatch(const std::string &name, const ArgumentList &args = {});

	virtual void clear();


	virtual void readState(Serialize::SerializeInStream &in) override;

protected:	

    virtual void collectValueRefs(std::list<ValueType *> &values);

    


private:

	ValueType execScriptMethod(const std::string &name, const ArgumentList &args);
	const std::list<std::string> &getArguments(const std::string &name);

	virtual const Parsing::MethodNodePtr &getMethod(const std::string &name);


private:
    ////API

    ValueType hasProperty(const ArgumentList &stack);
    ValueType set(const ArgumentList &stack);
    ValueType get(const ArgumentList &stack);
    ValueType checkFlag(const ArgumentList &stack);
	ValueType findPrototype(const ArgumentList &stack);
	ValueType getPrototype(const ArgumentList &stack);
	ValueType setPrototype(const ArgumentList &stack);

private:
    typedef ValueType(Scope::*NativeMethod)(const ArgumentList &);

    static const std::map<std::string, NativeMethod> mBoundNativeMethods;

	Serialize::Observed<std::string> mPrototypeName;
	Scope *mPrototype;
	
	VarSet mVariables;
};

}
}


