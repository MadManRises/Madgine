#include "madginelib.h"
#include "valuetype.h"
#include "Scripting/scriptingexception.h"
#include "Database\exceptionmessages.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\refscope.h"
#include "Serialize\serializeexception.h"
#include "Serialize\Streams\serializestream.h"

namespace Engine {

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
    mType(other.mType),
	mUnion(other.mUnion)
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

ValueType::ValueType(Scripting::Scope *e) :
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

ValueType::ValueType(InvPtr s) :
	mType(Type::InvPtrValue)
{
	mUnion.mInvPtr = s;
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

void ValueType::operator=(Scripting::Scope *e)
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

void ValueType::operator=(InvPtr s) {
	setType(Type::InvPtrValue);
	mUnion.mInvPtr = s;
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
        MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::invalidValueType));
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
    throw Scripting::ScriptingException(Database::Exceptions::invalidValueType);
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
    MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("+", getTypeString(), other.getTypeString())));
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
	MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("-", getTypeString(), other.getTypeString())));
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
	case Type::FloatValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mFloat /= other.mUnion.mInt;
			return;
		case Type::FloatValue:
			mUnion.mFloat /= other.mUnion.mFloat;
			return;
		}
		break;
	}
	MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("/", getTypeString(), other.getTypeString())));
}

ValueType ValueType::operator/(const ValueType &other) const
{
	ValueType result = *this;
	result /= other;
	return result;
}

void ValueType::operator*=(const ValueType &other)
{
	switch (mType) {
	case Type::IntValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mInt *= other.mUnion.mInt;
			return;
		case Type::FloatValue:
			(*this) = mUnion.mInt * other.mUnion.mFloat;
			return;
		}
		break;
	case Type::FloatValue:
		switch (other.mType) {
		case Type::IntValue:
			mUnion.mFloat *= other.mUnion.mInt;
			return;
		case Type::FloatValue:
			mUnion.mFloat *= other.mUnion.mFloat;
			return;
		}
		break;
	}
	MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("*", getTypeString(), other.getTypeString())));
}

ValueType ValueType::operator*(const ValueType &other) const
{
	ValueType result = *this;
	result *= other;
	return result;
}


const Ogre::Vector2 &ValueType::asVector2() const
{
	if (mType != Type::Vector2Value) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Vector2")));
	return *mUnion.mVector2;
}

const Ogre::Vector2 &ValueType::asVector2(const Ogre::Vector2 &v)
{
	if (mType != Type::Vector2Value) {
		if (mType == Type::NullValue)
			*this = v;
		else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return *mUnion.mVector2;
}

const Ogre::Vector3 &ValueType::asVector3() const
{
    if (mType != Type::Vector3Value) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Vector3")));
    return *mUnion.mVector3;
}

const Ogre::Vector3 &ValueType::asVector3(const Ogre::Vector3 &v)
{
    if (mType != Type::Vector3Value) {
        if (mType == Type::NullValue)
            *this = v;
        else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return *mUnion.mVector3;
}

bool ValueType::isScope() const
{
    return mType == Type::ScopeValue;
}

Scripting::Scope *ValueType::asScope() const
{
    if (mType != Type::ScopeValue) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Scope")));
    return mUnion.mScope;
}

Scripting::Scope * ValueType::asScope(Scripting::Scope * s)
{
	if (mType != Type::ScopeValue) {
		if (mType == Type::NullValue) {
			*this = s;
		}
		else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return mUnion.mScope;
}

bool ValueType::isInvScope() const
{
    return mType == Type::InvPtrValue;
}

InvPtr ValueType::asInvScope() const
{
    if (mType != Type::InvPtrValue) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("InvScope")));
    return mUnion.mInvPtr;
}

InvPtr ValueType::asInvScope(InvPtr s)
{
	if (mType != Type::InvPtrValue) {
		if (mType == Type::NullValue)
			*this = s;
		else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return mUnion.mInvPtr;
}

bool ValueType::isString() const
{
    return mType == Type::StringValue;
}

const std::string &ValueType::asString() const
{
    if (mType != Type::StringValue)
		MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Database::Exceptions::notValueType("String")));
    return *mUnion.mString;
}

const std::string &ValueType::asString(const std::string &s)
{
    if (mType != Type::StringValue) {
        if (mType == Type::NullValue)
            *this = s;
        else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return *mUnion.mString;
}

bool ValueType::isBool() const
{
    return mType == Type::BoolValue;
}

bool ValueType::asBool() const
{
    if (mType != Type::BoolValue) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Bool")));
    return mUnion.mBool;
}

bool ValueType::asBool(bool b)
{
    if (mType != Type::BoolValue) {
        if (mType == Type::NullValue)
            *this = b;
        else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
    }
    return mUnion.mBool;
}

bool ValueType::isUInt() const
{
	return mType == Type::UIntValue;
}

size_t ValueType::asUInt() const
{
	if (mType == Type::IntValue && mUnion.mInt >= 0) return mUnion.mInt;
	if (mType != Type::UIntValue) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Unsigned Int")));
	return mUnion.mUInt;
}

size_t ValueType::asUInt(size_t s)
{
	if (mType != Type::UIntValue) {
		if (mType == Type::NullValue)
			*this = s;
		else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
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
		MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Int")));
    return mUnion.mInt;
}

int ValueType::asInt(int i)
{
    if (mType != Type::IntValue) {
        if (mType == Type::NullValue)
            *this = i;
        else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
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
            && mType != Type::IntValue) MADGINE_THROW(Scripting::ScriptingException(Database::Exceptions::notValueType("Float")));
    if (mType == Type::IntValue) return mUnion.mInt;
    return mUnion.mFloat;
}

float ValueType::asFloat(float f)
{
    if (mType != Type::FloatValue && mType != Type::IntValue) {
        if (mType == Type::NullValue)
            *this = f;
        else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
    }
    if (mType == Type::IntValue) return mUnion.mInt;
    return mUnion.mFloat;
}

bool ValueType::isNull() const
{
    return mType == Type::NullValue;
}

bool ValueType::isEOL() const
{
	return mType == Type::EndOfListValue;
}

const ValueType & ValueType::asDefault(const ValueType & def)
{
	if (mType != def.mType) {
		if (mType == Type::NullValue)
			*this = def;
		else
			MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
	}
	return *this;
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
	case Type::EndOfListValue:
		return "EOL";
    case Type::ScopeValue:
        return mUnion.mScope->getIdentifier();
    case Type::FloatValue:
        return std::to_string(mUnion.mFloat);
    default:
		MADGINE_THROW(Scripting::ScriptingException("Unknown Type!"));
    }


}

std::string ValueType::getTypeString() const
{
	switch (mType) {
	case Type::BoolValue:
		return "Bool";
	case Type::EndOfListValue:
		return "EOL-Type";
	case Type::FloatValue:
		return "Float";
	case Type::IntValue:
		return "Integer";
	case Type::InvPtrValue:
		return "Invalid Scope";
	case Type::NullValue:
		return "Null-Type";
	case Type::ScopeValue:
		return "Scope";
	case Type::StringValue:
		return "String";
	case Type::UIntValue:
		return "Unsigned Integer";
	case Type::Vector2Value:
		return "Vector2";
	case Type::Vector3Value:
		return "Vector3";
	}
	throw 0;
}

void ValueType::readState(Serialize::SerializeInStream & in)
{
	clear();
	in.read(mType);

	switch (mType) {
	case Type::BoolValue:
		in.read(mUnion.mBool);
		break;
	case Type::StringValue: {
		decltype(std::declval<std::string>().size()) size;
		in.read(size);
		std::string *temp = new std::string(size, ' ');
		in.read(&(*temp)[0], size);
		mUnion.mString = temp;
		break;
	}
	case Type::IntValue:
		in.read(mUnion.mInt);
		break;
	case Type::UIntValue:
		in.read(mUnion.mUInt);
		break;
	case Type::NullValue:
		break;
	case Type::EndOfListValue:
		break;
	case Type::Vector3Value:
		float buffer[3];
		in.read(buffer);
		mUnion.mVector3 = new Ogre::Vector3(buffer);
		break;
	case Type::FloatValue:
		in.read(mUnion.mFloat);
		break;
	case Type::InvPtrValue:
		in.read(mUnion.mInvPtr);
		break;
	default:
		throw Serialize::SerializeException(Database::Exceptions::unknownDeserializationType);
	}
}

void ValueType::writeState(Serialize::SerializeOutStream & out) const
{
	out.write(mType == Type::ScopeValue ? Type::InvPtrValue : mType);
	switch (mType) {
	case Type::BoolValue:
		out.write(mUnion.mBool);
		break;
	case Type::StringValue: {
		auto size = mUnion.mString->size();
		out.write(size);
		out.writeData(mUnion.mString->c_str(), size);
		break;
	}
	case Type::IntValue:
		out.write(mUnion.mInt);
		break;
	case Type::UIntValue:
		out.write(mUnion.mUInt);
		break;
	case Type::NullValue:
		break;
	case Type::EndOfListValue:
		break;
	case Type::ScopeValue:
		out.write(mUnion.mScope);
		break;
	case Type::Vector3Value:
		out.writeData(mUnion.mVector3->ptr(), sizeof(float) * 3);
		break;
	case Type::FloatValue:
		out.write(mUnion.mFloat);
		break;
	case Type::InvPtrValue:
		out.write(mUnion.mInvPtr);
		break;
	default:
		throw Serialize::SerializeException(Database::Exceptions::unknownSerializationType);
	}
}

void ValueType::applySerializableMap(const std::map<InvPtr, Serialize::SerializableUnit*>& map)
{
	if (mType == Type::InvPtrValue) {
		auto it = map.find(mUnion.mInvPtr);
		if (it != map.end()) {
			Scripting::Scope *scope = dynamic_cast<Scripting::Scope*>(it->second);
			if (scope) {
				*this = scope;
			}
			else {
				LOG_WARNING("Non-Scope InvPtr in ValueType!");
			}
		}
		else {
			LOG_WARNING("Unmapped InvPtr!");
		}
	}
}

bool ValueType::peek(Serialize::SerializeInStream & in)
{
	Serialize::pos_type pos = in.tell();
	readState(in);
	if (isEOL())
		return false;
	in.seek(pos);
	return true;
}

ValueType ValueType::EOL()
{
	ValueType v;
	v.mType = Type::EndOfListValue;
	return v;
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
	Scripting::RefScope *s = dynamic_cast<Scripting::RefScope *>(mUnion.mScope);
    if (s)
        s->unref();
}

void ValueType::incRef()
{
    assert(mType == Type::ScopeValue);
	Scripting::RefScope *s = dynamic_cast<Scripting::RefScope *>(mUnion.mScope);
    if (s)
        s->ref();
}

template <>
MADGINE_EXPORT const std::string &ValueType::as<std::string>() const{
    return asString();
}

template <>
MADGINE_EXPORT const Ogre::Vector3 &ValueType::as<Ogre::Vector3>() const{
    return asVector3();
}

template <>
MADGINE_EXPORT const Ogre::Vector2 &ValueType::as<Ogre::Vector2>() const {
	return asVector2();
}

template <>
MADGINE_EXPORT float ValueType::as<float>() const{
    return asFloat();
}

template <>
MADGINE_EXPORT bool ValueType::as<bool>() const{
    return asBool();
}

template <>
MADGINE_EXPORT int ValueType::as<int>() const{
    return asInt();
}

template <>
MADGINE_EXPORT size_t ValueType::as<size_t>() const {
	return asUInt();
}

template <>
MADGINE_EXPORT InvPtr ValueType::as<InvPtr>() const {
	return asInvScope();
}

template <>
MADGINE_EXPORT Scripting::Scope *ValueType::as<Scripting::Scope*>() const {
	return asScope();
}

template <>
MADGINE_EXPORT const ValueType &ValueType::as<ValueType>() const {
	return *this;
}

template <>
MADGINE_EXPORT const std::string &ValueType::asDefault<std::string>(const std::string &s) {
	return asString(s);
}

template <>
MADGINE_EXPORT const Ogre::Vector3 &ValueType::asDefault<Ogre::Vector3>(const Ogre::Vector3 &v) {
	return asVector3(v);
}

template <>
MADGINE_EXPORT const Ogre::Vector2 &ValueType::asDefault<Ogre::Vector2>(const Ogre::Vector2 &v) {
	return asVector2(v);
}

template <>
MADGINE_EXPORT float ValueType::asDefault<float>(float f) {
	return asFloat(f);
}

template <>
MADGINE_EXPORT bool ValueType::asDefault<bool>(bool b) {
	return asBool(b);
}

template <>
MADGINE_EXPORT int ValueType::asDefault<int>(int i) {
	return asInt(i);
}

template <>
MADGINE_EXPORT size_t ValueType::asDefault<size_t>(size_t i) {
	return asUInt(i);
}

template <>
MADGINE_EXPORT InvPtr ValueType::asDefault<InvPtr>(InvPtr s) {
	return asInvScope(s);
}

template <>
MADGINE_EXPORT Scripting::Scope *ValueType::asDefault<Scripting::Scope*>(Scripting::Scope *s) {
	return asScope(s);
}

template <>
MADGINE_EXPORT const ValueType &ValueType::asDefault<ValueType>(const ValueType &v) {
	return asDefault(v);
}


}


std::ostream &operator <<(std::ostream &stream,
	const Engine::ValueType &v)
{
	return stream << v.toString();
}