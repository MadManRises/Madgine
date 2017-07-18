#include "interfaceslib.h"
#include "valuetype.h"

#include "scripting/types/api.h"

#include "serialize/serializeexception.h"


namespace Engine {

	void Serialize::UnitHelper<ValueType, true>::applyMap(const std::map<size_t, SerializableUnitBase*> &map, ValueType &item)
	{
		/*if (item.isInvPtr()) {
			auto it = map.find(static_cast<size_t>(item.asInvPtr()));
			if (it != map.end()) {
				Scripting::ScopeBase *scope = dynamic_cast<Scripting::ScopeBase*>(it->second);
				if (scope) {
					item = scope;
				}
				else {
					LOG_WARNING("Non-Scope InvScopePtr in ValueType!");
				}
			}
			else {
				LOG_WARNING("Unmapped InvScopePtr!");
			}
		}*/
	}


	template <>
	INTERFACES_EXPORT bool ValueType::is<std::string>() const {
		return isString();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<std::array<float, 4>>() const {
		return isVector4();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<std::array<float, 3>>() const {
		return isVector3();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<std::array<float, 2>>() const {
		return isVector2();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<float>() const {
		return isFloat();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<bool>() const {
		return isBool();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<int>() const {
		return isInt();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<InvScopePtr>() const {
		return isInvPtr();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<Scripting::ScopeBase*>() const {
		return isScope();
	}

	template <>
	INTERFACES_EXPORT bool ValueType::is<ValueType>() const {
		return true;
	}

template <>
INTERFACES_EXPORT const std::string &ValueType::as<std::string>() const{
    return asString();
}

template <>
INTERFACES_EXPORT const std::array<float, 4> &ValueType::as<std::array<float, 4>>() const {
	return asVector4();
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
INTERFACES_EXPORT const InvScopePtr &ValueType::as<InvScopePtr>() const {
	return asInvPtr();
}

template <>
INTERFACES_EXPORT Scripting::ScopeBase *ValueType::as<Scripting::ScopeBase*>() const {
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
INTERFACES_EXPORT const InvScopePtr &ValueType::asDefault<InvScopePtr>(const InvScopePtr &s) {
	return asInvPtr(s);
}

template <>
INTERFACES_EXPORT Scripting::ScopeBase *ValueType::asDefault<Scripting::ScopeBase*>(Scripting::ScopeBase *s) {
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
	case Type::UIntValue:
		return std::to_string(mUnion.mUInt);
	case Type::NullValue:
		return "NULL";
	case Type::EndOfListValue:
		return "EOL";
	case Type::ScopeValue:
		return mUnion.mScope->getIdentifier();
	case Type::FloatValue:
		return std::to_string(mUnion.mFloat);
	case Type::Vector3Value:
		return std::string("[") + std::to_string(mUnion.mVector3[0]) + ", " + std::to_string(mUnion.mVector3[1]) + ", " + std::to_string(mUnion.mVector3[2]) + "]";
	default:
		MADGINE_THROW(Scripting::ScriptingException("Unknown Type!"));
	}


}

}


std::ostream &operator <<(std::ostream &stream,
	const Engine::ValueType &v)
{
	return stream << v.toString();
}
