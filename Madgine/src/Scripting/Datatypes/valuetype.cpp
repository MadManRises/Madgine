#include "libinclude.h"
#include "valuetype.h"
#include "Scripting/scriptingexception.h"
#include "Database\exceptionmessages.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\refscope.h"

namespace Engine {
namespace Scripting {

ValueType::ValueType() :
    mType(Type::NullValue)
{

}

ValueType::ValueType(const ValueType &other) :
    mType(other.mType)
{
    if (mType == Type::StringValue) {
        mUnion.mString = new std::string(*other.mUnion.mString);
	}
	else if (mType == Type::Vector3Value) {
		mUnion.mVector3 = new Ogre::Vector3(*other.mUnion.mVector3);
	} else if (mType == Type::Vector2Value) {
		mUnion.mVector2 = new Ogre::Vector2(*other.mUnion.mVector2);
    } else {
        mUnion = other.mUnion;
    }
    if (mType == Type::ScopeValue)
        incRef();
}

ValueType::ValueType(ValueType &&other) :
    mUnion(other.mUnion),
    mType(other.mType)
{
    other.mType = Type::NullValue;
}

ValueType::ValueType(const Ogre::Vector2 & v) :
	mType(Type::Vector2Value)
{
	mUnion.mVector2 = new Ogre::Vector2(v);
}

ValueType::ValueType(const Ogre::Vector3 &v) :
    mType(Type::Vector3Value)
{
    mUnion.mVector3 = new Ogre::Vector3(v);
}

ValueType::ValueType(Scope *e) :
    mType(Type::ScopeValue)
{
    mUnion.mScope = e;
    incRef();
}

ValueType::ValueType(const std::string &s) :
    mType(Type::StringValue)
{
    mUnion.mString = new std::string(s);
}

ValueType::ValueType(const char *s) :
    mType(Type::StringValue)
{
    mUnion.mString = new std::string(s);
}

ValueType::ValueType(bool b) :
    mType(Type::BoolValue)
{
    mUnion.mBool = b;
}

ValueType::ValueType(int i) :
    mType(Type::IntValue)
{
    mUnion.mInt = i;
}

ValueType::ValueType(float f) :
    mType(Type::FloatValue)
{
    mUnion.mFloat = f;
}

ValueType::ValueType(InvScopePtr s) :
	mType(Type::InvScopeValue)
{
	mUnion.mInvScope = s;
}

ValueType::ValueType(size_t s) :
	mType(Type::UIntValue)
{
	mUnion.mUInt = s;
}

ValueType::~ValueType()
{
    clear();
}

void ValueType::clear()
{
    setType(Type::NullValue);
}

void ValueType::operator=(const ValueType &other)
{
    if (setType(other.mType)){
        if (mType == Type::StringValue)
            mUnion.mString = new std::string(*other.mUnion.mString);
		else if (mType == Type::Vector3Value)
			mUnion.mVector3 = new Ogre::Vector3(*other.mUnion.mVector3);
		else if (mType == Type::Vector2Value)
			mUnion.mVector2 = new Ogre::Vector2(*other.mUnion.mVector2);
        else
            mUnion = other.mUnion;
    }else{
        if (mType == Type::StringValue)
            *mUnion.mString = *other.mUnion.mString;
		else if (mType == Type::Vector3Value)
			*mUnion.mVector3 = *other.mUnion.mVector3;
		else if (mType == Type::Vector2Value)
			*mUnion.mVector2 = *other.mUnion.mVector2;
        else
            mUnion = other.mUnion;
    }
    if (mType == Type::ScopeValue)
        incRef();
}

void ValueType::operator=(Scope *e)
{
    setType(Type::ScopeValue);
    mUnion.mScope = e;
    incRef();
}

void ValueType::operator=(const std::string &s)
{
    if (setType(Type::StringValue)){
        mUnion.mString = new std::string(s);
    } else {
        *mUnion.mString = s;
    }
}

void ValueType::operator=(const char *s)
{
    *this = std::string(s);
}

void ValueType::operator=(bool b)
{
    setType(Type::BoolValue);
    mUnion.mBool = b;
}

void ValueType::operator=(int i)
{
    setType(Type::IntValue);
    mUnion.mInt = i;
}

void ValueType::operator=(size_t s) 
{
	setType(Type::UIntValue);
	mUnion.mUInt = s;
}

void ValueType::operator=(float f)
{
    setType(Type::FloatValue);
    mUnion.mFloat = f;    
}

void ValueType::operator=(InvScopePtr s) {
	setType(Type::InvScopeValue);
	mUnion.mInvScope = s;
}

bool ValueType::operator==(const ValueType &other) const
{
    if (other.mType != mType)
        return false;
    switch (mType){
    case Type::StringValue:
        return *mUnion.mString == *other.mUnion.mString;
    case Type::BoolValue:
        return mUnion.mBool == other.mUnion.mBool;
    case Type::ScopeValue:
        return mUnion.mScope == other.mUnion.mScope;
	case Type::IntValue:
        return mUnion.mInt == other.mUnion.mInt;
    case Type::FloatValue:
        return mUnion.mFloat == other.mUnion.mFloat;
    case Type::Vector3Value:
        return *mUnion.mVector3 == *other.mUnion.mVector3;
	case Type::Vector2Value:
		return *mUnion.mVector2 == *other.mUnion.mVector2;
    case Type::NullValue:
        return true;
    default:
        MADGINE_THROW(ScriptingException(Database::Exceptions::invalidValueType));
    }
}

bool ValueType::operator!=(const ValueType &other) const
{
	return !(*this == other);
}

bool ValueType::operator <(const ValueType &other) const
{
    switch(mType){
    case Type::StringValue:
        if (other.mType == Type::StringValue){
            return *mUnion.mString < *other.mUnion.mString;
        }
        break;
    case Type::IntValue:
        if (other.mType == Type::IntValue){
            return mUnion.mInt < other.mUnion.mInt;
		}
		else if (other.mType == Type::UIntValue) {
			return mUnion.mInt < other.mUnion.mUInt;
		}else if (other.mType == Type::FloatValue) {
            return mUnion.mInt < other.mUnion.mFloat;
        }
        break;
    }
    throw ScriptingException(Database::Exceptions::invalidValueType);
}

bool ValueType::operator >(const ValueType &other) const
{
    return other < *this;
}

void ValueType::operator+=(const ValueType &other)
{
    switch(mType){
    case Type::StringValue:
        switch(other.mType){
		case Type::StringValue:
            *mUnion.mString += *other.mUnion.mString;
            return;
		case Type::IntValue:
			*mUnion.mString += std::to_string(other.mUnion.mInt);
			return;
        }
        break;
    case Type::IntValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mInt += other.mUnion.mInt;
			return;
		case Type::FloatValue:
			(*this) = mUnion.mInt + other.mUnion.mFloat;
			return;
		}        
        break;
    }
    throw 0;
}

ValueType ValueType::operator+(const ValueType &other) const
{
	ValueType result = *this;
	result += other;
	return result;
}

void ValueType::operator-=(const ValueType &other)
{
	switch (mType) {
	case Type::IntValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mInt -= other.mUnion.mInt;
			return;
		case Type::FloatValue:
			(*this) = mUnion.mInt - other.mUnion.mFloat;
			return;
		}
		break;
	}
	throw 0;
}

ValueType ValueType::operator-(const ValueType &other) const
{
	ValueType result = *this;
	result -= other;
	return result;
}

void ValueType::operator/=(const ValueType &other)
{
	switch (mType) {
	case Type::IntValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mInt /= other.mUnion.mInt;
			return;
		case Type::FloatValue:
			(*this) = mUnion.mInt / other.mUnion.mFloat;
			return;
		}
		break;
	}
	throw 0;
}

ValueType ValueType::operator/(const ValueType &other) const
{
	ValueType result = *this;
	result /= other;
	return result;
}


const Ogre::Vector2 &ValueType::asVector2() const
{
	if (mType != Type::Vector2Value) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Vector2")));
	return *mUnion.mVector2;
}

const Ogre::Vector2 &ValueType::asVector2(const Ogre::Vector2 &v)
{
	if (mType != Type::Vector2Value) {
		if (mType == Type::NullValue)
			*this = v;
		else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return *mUnion.mVector2;
}

const Ogre::Vector3 &ValueType::asVector3() const
{
    if (mType != Type::Vector3Value) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Vector3")));
    return *mUnion.mVector3;
}

const Ogre::Vector3 &ValueType::asVector3(const Ogre::Vector3 &v)
{
    if (mType != Type::Vector3Value) {
        if (mType == Type::NullValue)
            *this = v;
        else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return *mUnion.mVector3;
}

bool ValueType::isScope() const
{
    return mType == Type::ScopeValue;
}

Scope *ValueType::asScope() const
{
    if (mType != Type::ScopeValue) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Scope")));
    return mUnion.mScope;
}

Scope * ValueType::asScope(const Scope * s)
{
	if (mType != Type::ScopeValue) {
		if (mType == Type::NullValue) {
			*this = s;
		}
		else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return mUnion.mScope;
}

bool ValueType::isInvScope() const
{
    return mType == Type::InvScopeValue;
}

InvScopePtr ValueType::asInvScope() const
{
    if (mType != Type::InvScopeValue) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("InvScope")));
    return mUnion.mInvScope;
}

InvScopePtr ValueType::asInvScope(InvScopePtr s)
{
	if (mType != Type::InvScopeValue) {
		if (mType == Type::NullValue)
			*this = s;
		else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return mUnion.mInvScope;
}

bool ValueType::isString() const
{
    return mType == Type::StringValue;
}

const std::string &ValueType::asString() const
{
    if (mType != Type::StringValue)
		MADGINE_THROW_NO_TRACE(ScriptingException(Database::Exceptions::notValueType("String")));
    return *mUnion.mString;
}

const std::string &ValueType::asString(const std::string &s)
{
    if (mType != Type::StringValue) {
        if (mType == Type::NullValue)
            *this = s;
        else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return *mUnion.mString;
}

bool ValueType::isBool() const
{
    return mType == Type::BoolValue;
}

bool ValueType::asBool() const
{
    if (mType != Type::BoolValue) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Bool")));
    return mUnion.mBool;
}

bool ValueType::asBool(bool b)
{
    if (mType != Type::BoolValue) {
        if (mType == Type::NullValue)
            *this = b;
        else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return mUnion.mBool;
}

bool ValueType::isUInt() const
{
	return mType == Type::UIntValue;
}

size_t ValueType::asUInt() const
{
	if (mType != Type::UIntValue) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Unsigned Int")));
	return mUnion.mUInt;
}

size_t ValueType::asUInt(size_t s)
{
	if (mType != Type::UIntValue) {
		if (mType == Type::NullValue)
			*this = s;
		else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return mUnion.mUInt;
}

bool ValueType::isInt() const
{
    return mType == Type::IntValue;
}

int ValueType::asInt() const
{
    if (mType != Type::IntValue) 
		MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Int")));
    return mUnion.mInt;
}

int ValueType::asInt(int i)
{
    if (mType != Type::IntValue) {
        if (mType == Type::NullValue)
            *this = i;
        else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return mUnion.mInt;
}

bool ValueType::isFloat() const
{
    return mType == Type::FloatValue;
}

float ValueType::asFloat() const
{
    if (mType != Type::FloatValue
            && mType != Type::IntValue) MADGINE_THROW(ScriptingException(Database::Exceptions::notValueType("Float")));
    if (mType == Type::IntValue) return mUnion.mInt;
    return mUnion.mFloat;
}

float ValueType::asFloat(float f)
{
    if (mType != Type::FloatValue && mType != Type::IntValue) {
        if (mType == Type::NullValue)
            *this = f;
        else
			MADGINE_THROW(ScriptingException("Can't assign default value to non-Null variable!"));
    }
    if (mType == Type::IntValue) return mUnion.mInt;
    return mUnion.mFloat;
}

bool ValueType::isNull() const
{
    return mType == Type::NullValue;
}

std::string ValueType::toString() const
{
    switch (mType) {
    case Type::BoolValue:
        return mUnion.mBool ? "true" : "false";
    case Type::StringValue:
        return std::string("\"") + *mUnion.mString + "\"";
    case Type::IntValue:
        return std::to_string(mUnion.mInt);
    case Type::NullValue:
        return "NULL";
    case Type::ScopeValue:
        return mUnion.mScope->getIdentifier();
    case Type::FloatValue:
        return std::to_string(mUnion.mFloat);
    default:
		MADGINE_THROW(ScriptingException("Unknown Type!"));
    }


}

bool ValueType::setType(ValueType::Type t)
{
    if (mType == Type::ScopeValue)
        decRef();
    if (mType == t) return false;
    if (mType == Type::StringValue)
        delete mUnion.mString;
	else if (mType == Type::Vector3Value)
		delete mUnion.mVector3;
	else if (mType == Type::Vector2Value)
		delete mUnion.mVector2;
    mType = t;
    return true;
}

void ValueType::decRef()
{
    assert(mType == Type::ScopeValue);
    RefScope *s = dynamic_cast<RefScope *>(mUnion.mScope);
    if (s)
        s->unref();
}

void ValueType::incRef()
{
    assert(mType == Type::ScopeValue);
    RefScope *s = dynamic_cast<RefScope *>(mUnion.mScope);
    if (s)
        s->ref();
}

template <>
MADGINE_EXPORT const std::string &ValueType::as<std::string>() const{
    return asString();
}

template <>
MADGINE_EXPORT const std::string &ValueType::asDefault<std::string>(const std::string &defaultValue){
    return asString(defaultValue);
}

template <>
MADGINE_EXPORT const Ogre::Vector3 &ValueType::as<Ogre::Vector3>() const{
    return asVector3();
}

template <>
MADGINE_EXPORT const Ogre::Vector3 &ValueType::asDefault<Ogre::Vector3>(const Ogre::Vector3 &defaultValue){
    return asVector3(defaultValue);
}

template <>
MADGINE_EXPORT const Ogre::Vector2 &ValueType::as<Ogre::Vector2>() const {
	return asVector2();
}

template <>
MADGINE_EXPORT const Ogre::Vector2 &ValueType::asDefault<Ogre::Vector2>(const Ogre::Vector2 &defaultValue) {
	return asVector2(defaultValue);
}

template <>
MADGINE_EXPORT float ValueType::as<float>() const{
    return asFloat();
}

template <>
MADGINE_EXPORT float ValueType::asDefault<float>(float defaultValue){
    return asFloat(defaultValue);
}

template <>
MADGINE_EXPORT bool ValueType::as<bool>() const{
    return asBool();
}

template <>
MADGINE_EXPORT bool ValueType::asDefault<bool>(bool defaultValue){
    return asBool(defaultValue);
}

template <>
MADGINE_EXPORT int ValueType::as<int>() const{
    return asInt();
}

template <>
MADGINE_EXPORT int ValueType::asDefault<int>(int defaultValue){
    return asInt(defaultValue);
}

template <>
MADGINE_EXPORT size_t ValueType::as<size_t>() const {
	return asUInt();
}

template <>
MADGINE_EXPORT size_t ValueType::asDefault<size_t>(size_t defaultValue) {
	return asInt(defaultValue);
}

template <>
MADGINE_EXPORT InvScopePtr ValueType::as<InvScopePtr>() const {
	return asInvScope();
}

template <>
MADGINE_EXPORT InvScopePtr ValueType::asDefault<InvScopePtr>(InvScopePtr defaultValue) {
	return asInvScope(defaultValue);
}

template <>
MADGINE_EXPORT Scope *ValueType::as<Scope*>() const {
	return asScope();
}

template <>
MADGINE_EXPORT Scope *ValueType::asDefault<Scope*>(Scope* defaultValue) {
	return asScope(defaultValue);
}


}
}


std::ostream &operator <<(std::ostream &stream,
	const Engine::Scripting::ValueType &v)
{
	return stream << v.toString();
}