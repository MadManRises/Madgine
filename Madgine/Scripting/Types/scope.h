#pragma once

#include "Scripting/Datatypes/varset.h"
#include "Scripting/Datatypes/argumentlist.h"

namespace Engine {
namespace Scripting {

enum class ScopeClass{
    EntityClass,
    ListClass,
    StructClass,
    LevelClass,
    StoryClass
};

template <class T>
struct ScopeClassType{};

template <>
struct ScopeClassType<Story> {
	static constexpr ScopeClass type = ScopeClass::StoryClass;
};

template <>
struct ScopeClassType<OGRE::Entity::Entity>{
	static constexpr ScopeClass type = ScopeClass::EntityClass;
};

template <>
struct ScopeClassType<List> {
	static constexpr ScopeClass type = ScopeClass::ListClass;
};

template <>
struct ScopeClassType<Struct> {
	static constexpr ScopeClass type = ScopeClass::StructClass;
};

template <>
struct ScopeClassType<Level> {
	static constexpr ScopeClass type = ScopeClass::LevelClass;
};


template <class T>
T *scope_cast(Scope *s) {
	if (s->getClassType() != ScopeClassType<T>::type) return 0;
	return (T*)s;
}

class MADGINE_EXPORT Scope : public Ogre::ScriptingAllocatedObject, public Serialize::Serializable {

public:
	Scope(Scope *data = nullptr);
    virtual ~Scope();

    void setVar(const std::string &name, const ValueType &v);
    const ValueType &getVar(const std::string &name) const;
    ValueType &accessVar(const std::string &name);
    bool hasVar(const std::string &name) const;
	bool checkFlag(const std::string &name, bool defaultValue = false);
    
	void findData(const std::string &name);
	void setData(Scope *data);


	virtual bool hasScriptMethod(const std::string &name);
	
	void applyScopeMap(const std::map<InvScopePtr, Scope *> &map);
	void collectScopes(std::set<Scope *> &scopeMap, const std::set<Scope *> &ignoreMap = {});
	virtual void collectNamedValues(std::map<std::string, ValueType*> &values);

	virtual void storeCreationData(Serialize::SerializeOutStream &of);

    virtual ScopeClass getClassType() = 0;
	virtual std::string getIdentifier() = 0;	

	virtual ValueType methodCall(const std::string &name,
		const ArgumentList &args = {});

	bool callMethodIfAvailable(const std::string &name, const ArgumentList &args = {});
	bool callMethodCatch(const std::string &name, const ArgumentList &args = {});

protected:	

	virtual void clear();

    virtual void collectValueRefs(std::list<ValueType *> &values);

    virtual void save(Scripting::Serialize::SerializeOutStream &of) const override;
    virtual void load(Scripting::Serialize::SerializeInStream &ifs) override;


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
	ValueType findData(const ArgumentList &stack);
	ValueType setData(const ArgumentList &stack);

private:
    typedef ValueType(Scope::*NativeMethod)(const ArgumentList &);

    static const std::map<std::string, NativeMethod> mBoundNativeMethods;

	VarSet mVariables;

	const Scope *mPrototype;
};


}
}


