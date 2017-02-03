#include "interfaceslib.h"
#include "valuetype.h"


#include "Scripting\Types\refscope.h"

#include "Serialize\serializeexception.h"


namespace Engine {

	void Serialize::UnitHelper<ValueType, true>::applyMap(const std::map<InvPtr, SerializableUnit*> &map, ValueType &item)
	{
		if (item.isInvScope()) {
			auto it = map.find(item.asInvPtr());
			if (it != map.end()) {
				Scripting::Scope *scope = dynamic_cast<Scripting::Scope*>(it->second);
				if (scope) {
					item = scope;
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

template <>
INTERFACES_EXPORT const std::string &ValueType::as<std::string>() const{
    return asString();
}

template <>
INTERFACES_EXPORT const std::array<float, 3> &ValueType::as<std::array<float, 3>>() const{
    return asVector3();
}

template <>
INTERFACES_EXPORT const std::array<float, 2> &ValueType::as<std::array<float, 2>>() const {
	return asVector2();
}

template <>
INTERFACES_EXPORT float ValueType::as<float>() const{
    return asFloat();
}

template <>
INTERFACES_EXPORT bool ValueType::as<bool>() const{
    return asBool();
}

template <>
INTERFACES_EXPORT int ValueType::as<int>() const{
    return asInt();
}

template <>
INTERFACES_EXPORT size_t ValueType::as<size_t>() const {
	return asUInt();
}

template <>
INTERFACES_EXPORT InvPtr ValueType::as<InvPtr>() const {
	return asInvPtr();
}

template <>
INTERFACES_EXPORT Scripting::Scope *ValueType::as<Scripting::Scope*>() const {
	return asScope();
}

template <>
INTERFACES_EXPORT const ValueType &ValueType::as<ValueType>() const {
	return *this;
}

template <>
INTERFACES_EXPORT const std::string &ValueType::asDefault<std::string>(const std::string &s) {
	return asString(s);
}

template <>
INTERFACES_EXPORT const std::array<float, 3> &ValueType::asDefault<std::array<float, 3>>(const std::array<float, 3> &v) {
	return asVector3(v);
}

template <>
INTERFACES_EXPORT const std::array<float, 2> &ValueType::asDefault<std::array<float, 2>>(const std::array<float, 2> &v) {
	return asVector2(v);
}

template <>
INTERFACES_EXPORT float ValueType::asDefault<float>(float f) {
	return asFloat(f);
}

template <>
INTERFACES_EXPORT bool ValueType::asDefault<bool>(bool b) {
	return asBool(b);
}

template <>
INTERFACES_EXPORT int ValueType::asDefault<int>(int i) {
	return asInt(i);
}

template <>
INTERFACES_EXPORT size_t ValueType::asDefault<size_t>(size_t i) {
	return asUInt(i);
}

template <>
INTERFACES_EXPORT InvPtr ValueType::asDefault<InvPtr>(InvPtr s) {
	return asInvPtr(s);
}

template <>
INTERFACES_EXPORT Scripting::Scope *ValueType::asDefault<Scripting::Scope*>(Scripting::Scope *s) {
	return asScope(s);
}

template <>
INTERFACES_EXPORT const ValueType &ValueType::asDefault<ValueType>(const ValueType &v) {
	return asDefault(v);
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

}


std::ostream &operator <<(std::ostream &stream,
	const Engine::ValueType &v)
{
	return stream << v.toString();
}