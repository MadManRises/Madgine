#pragma once

#include "Scripting/Datatypes/varset.h"
#include "Scripting\Datatypes\argumentlist.h"
#include "Util\UtilMethods.h"

namespace Engine {
namespace Scripting {

enum class ScopeClass{
    EntityClass,
    ListClass,
    StructClass,
    LevelClass,
    StoryClass,
	ArrayClass
};

template <class T>
struct ScopeClassType{};

template <>
struct ScopeClassType<GlobalScope> {
	static constexpr ScopeClass type = ScopeClass::StoryClass;
};

template <>
struct ScopeClassType<Engine::Scene::Entity::Entity>{
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
struct ScopeClassType<Scene> {
	static constexpr ScopeClass type = ScopeClass::LevelClass;
};

template <>
struct ScopeClassType<Array> {
	static constexpr ScopeClass type = ScopeClass::ArrayClass;
};




class MADGINE_EXPORT Scope : public Ogre::ScriptingAllocatedObject, public Serialize::Serializable {

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
	
	void applyScopeMap(const std::map<InvScopePtr, Scope *> &map);
	void collectScopes(std::set<Scope *> &scopeMap, const std::set<Scope *> &ignoreMap = {});
	//virtual void collectNamedValues(std::map<std::string, ValueType*> &values);

	virtual void storeCreationData(Serialize::SerializeOutStream &of);

    virtual ScopeClass getClassType() = 0;
	virtual std::string getIdentifier() = 0;	

	virtual ValueType methodCall(const std::string &name,
		const ArgumentList &args = {});

	bool callMethodIfAvailable(const std::string &name, const ArgumentList &args = {});
	bool callMethodCatch(const std::string &name, const ArgumentList &args = {});

	virtual void clear();

protected:	

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
	ValueType findPrototype(const ArgumentList &stack);
	ValueType getPrototype(const ArgumentList &stack);
	ValueType setPrototype(const ArgumentList &stack);

private:
    typedef ValueType(Scope::*NativeMethod)(const ArgumentList &);

    static const std::map<std::string, NativeMethod> mBoundNativeMethods;

	VarSet mVariables;

	Scope *mPrototype;
	std::string mPrototypeName;
};

template <class T>
T *scope_cast(Scope *s) {
	if (s->getClassType() != ScopeClassType<T>::type)
		MADGINE_THROW_NO_TRACE(ScriptingException(Database::Exceptions::unexpectedScopeType(s->getIdentifier(), typeid(T).name())));
	return (T*)s;
}


}
}


