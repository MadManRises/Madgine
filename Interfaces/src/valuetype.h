#pragma once

#include "Util\UtilMethods.h"
#include "Scripting/scriptingexception.h"
#include "exceptionmessages.h"

namespace Engine {

	template <class T>
	struct externValueType {
		const constexpr static bool value = false;
	};

	template <class T>
	struct _isValueType {
		const constexpr static bool value = false;
	};

	template <>
	struct _isValueType<int> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<std::string> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<std::array<float, 3>> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<std::array<float, 2>> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<float> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<bool> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<size_t> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<InvPtr> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<Scripting::Scope *> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<ValueType> {
		const constexpr static bool value = true;
	};

	template <class T>
	struct isValueType {
		const constexpr static bool value = _isValueType<std::remove_const_t<T>>::value || std::is_enum<T>::value;
	};


class INTERFACES_EXPORT ValueType {
public:
	ValueType() :
		mType(Type::NullValue)
	{

	}
	ValueType(const ValueType &other) :
		mType(other.mType)
	{
		if (mType == Type::StringValue) {
			mUnion.mString = new std::string(*other.mUnion.mString);
		}
		else {
			mUnion = other.mUnion;
		}
		if (mType == Type::ScopeValue)
			incRef();
	}
	ValueType(ValueType &&other) :
		mType(other.mType),
		mUnion(other.mUnion)
	{
		other.mType = Type::NullValue;
	}

	ValueType(Scripting::Scope *e) :
		mType(Type::ScopeValue)
	{
		mUnion.mScope = e;
		incRef();
	}
	ValueType(const std::string &s) :
		mType(Type::StringValue)
	{
		mUnion.mString = new std::string(s);
	}
	ValueType(const char *s) :
		mType(Type::StringValue)
	{
		mUnion.mString = new std::string(s);
	}
	ValueType(bool b) :
		mType(Type::BoolValue)
	{
		mUnion.mBool = b;
	}
	ValueType(int i) :
		mType(Type::IntValue)
	{
		mUnion.mInt = i;
	}
	ValueType(float f) :
		mType(Type::FloatValue)
	{
		mUnion.mFloat = f;
	}
	ValueType(const std::array<float, 3> &v) :
		mType(Type::Vector3Value)
	{
		mUnion.mVector3 = v;
	}
	ValueType(const std::array<float, 2> &v) :
		mType(Type::Vector2Value)
	{
		mUnion.mVector2 = v;
	}
	ValueType(InvPtr s) :
		mType(Type::InvPtrValue)
	{
		mUnion.mInvPtr = s;
	}
	ValueType(size_t s) :
		mType(Type::UIntValue)
	{
		mUnion.mUInt = s;
	}

	template <class T, class _ = std::enable_if_t<externValueType<T>::value>>
	ValueType(const T &);

	template <class T, class _ = std::enable_if_t<std::is_enum<T>::value>>
	ValueType(T val) :
		ValueType(static_cast<int>(val)) {}

	~ValueType()
	{
		clear();
	}

	void clear()
	{
		setType(Type::NullValue);
	}

    void operator=(const ValueType &other)
	{
		if (setType(other.mType)) {
			if (mType == Type::StringValue)
				mUnion.mString = new std::string(*other.mUnion.mString);
			else
				mUnion = other.mUnion;
		}
		else {
			if (mType == Type::StringValue)
				*mUnion.mString = *other.mUnion.mString;
			else
				mUnion = other.mUnion;
		}
		if (mType == Type::ScopeValue)
			incRef();
	}
    void operator=(Scripting::Scope *s)
	{
		setType(Type::ScopeValue);
		mUnion.mScope = s;
		incRef();
	}
    void operator=(const std::string &s)
	{
		if (setType(Type::StringValue)) {
			mUnion.mString = new std::string(s);
		}
		else {
			*mUnion.mString = s;
		}
	}
    void operator=(bool b)
	{
		setType(Type::BoolValue);
		mUnion.mBool = b;
	}
    void operator=(int i)
	{
		setType(Type::IntValue);
		mUnion.mInt = i;
	}
    void operator=(float f)
	{
		setType(Type::FloatValue);
		mUnion.mFloat = f;
	}
	void operator=(const std::array<float, 2> &v)
	{
		setType(Type::Vector2Value);
		mUnion.mVector2 = v;
	}
	void operator=(const std::array<float, 3> &v)
	{
		setType(Type::Vector3Value);
		mUnion.mVector3 = v;
	}
	void operator=(const char *s)
	{
		*this = std::string(s);
	}
	void operator=(InvPtr p)
	{
		setType(Type::InvPtrValue);
		mUnion.mInvPtr = p;
	}
	void operator=(size_t s)
	{
		setType(Type::UIntValue);
		mUnion.mUInt = s;
	}

    bool operator==(const ValueType &other) const
	{
		if (other.mType != mType)
			return false;
		switch (mType) {
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
			return mUnion.mVector3 == other.mUnion.mVector3;
		case Type::Vector2Value:
			return mUnion.mVector2 == other.mUnion.mVector2;
		case Type::NullValue:
			return true;
		default:
			MADGINE_THROW(Scripting::ScriptingException(Exceptions::invalidValueType));
		}
	}
	bool operator!=(const ValueType &other) const
	{
		return !(*this == other);
	}
    bool operator <(const ValueType &other) const
	{
		switch (mType) {
		case Type::StringValue:
			if (other.mType == Type::StringValue) {
				return *mUnion.mString < *other.mUnion.mString;
			}
			break;
		case Type::IntValue:
			if (other.mType == Type::IntValue) {
				return mUnion.mInt < other.mUnion.mInt;
			}
			else if (other.mType == Type::UIntValue) {
				return mUnion.mInt < other.mUnion.mUInt;
			}
			else if (other.mType == Type::FloatValue) {
				return mUnion.mInt < other.mUnion.mFloat;
			}
			break;
		}
		throw Scripting::ScriptingException(Exceptions::invalidValueType);
	}
    bool operator >(const ValueType &other) const
	{
		return other < *this;
	}

    void operator+=(const ValueType &other)
	{
		switch (mType) {
		case Type::StringValue:
			switch (other.mType) {
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
		MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("+", getTypeString(), other.getTypeString())));
	}

	ValueType operator+(const ValueType &other) const
	{
		ValueType result = *this;
		result += other;
		return result;
	}

	void operator-=(const ValueType &other) {
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
		MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("-", getTypeString(), other.getTypeString())));
	}
	ValueType operator-(const ValueType &other) const
	{
		ValueType result = *this;
		result -= other;
		return result;
	}

	void operator/=(const ValueType &other)
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
		MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("/", getTypeString(), other.getTypeString())));
	}

	ValueType operator/(const ValueType &other) const
	{
		ValueType result = *this;
		result /= other;
		return result;
	}

	void operator*=(const ValueType &other)
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
		MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("*", getTypeString(), other.getTypeString())));
	}

	ValueType operator*(const ValueType &other) const
	{
		ValueType result = *this;
		result *= other;
		return result;
	}



	const std::array<float, 2> &asVector2() const
	{
		if (mType != Type::Vector2Value) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Vector2")));
		return mUnion.mVector2;
	}
	const std::array<float, 2> &asVector2(const std::array<float, 2> &v)
	{
		if (mType != Type::Vector2Value) {
			if (mType == Type::NullValue)
				*this = v;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mVector2;
	}
	const std::array<float, 3> &asVector3() const
	{
		if (mType != Type::Vector3Value) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Vector3")));
		return mUnion.mVector3;
	}
	const std::array<float, 3> &asVector3(const std::array<float, 3> &v)
	{
		if (mType != Type::Vector3Value) {
			if (mType == Type::NullValue)
				*this = v;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mVector3;
	}
	bool isScope() const
	{
		return mType == Type::ScopeValue;
	}
	Scripting::Scope *asScope() const
	{
		if (mType != Type::ScopeValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Scope")));
		return mUnion.mScope;
	}
	Scripting::Scope *asScope(Scripting::Scope *s)
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
	bool isInvScope() const
	{
		return mType == Type::InvPtrValue;
	}
	InvPtr asInvPtr() const
	{
		if (mType != Type::InvPtrValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("InvScope")));
		return mUnion.mInvPtr;
	}
	InvPtr asInvPtr(InvPtr s)
	{
		if (mType != Type::InvPtrValue) {
			if (mType == Type::NullValue)
				*this = s;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mInvPtr;
	}
	bool isString() const
	{
		return mType == Type::StringValue;
	}
	const std::string &asString() const
	{
		if (mType != Type::StringValue)
			MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::notValueType("String")));
		return *mUnion.mString;
	}
	const std::string &asString(const std::string &s)
	{
		if (mType != Type::StringValue) {
			if (mType == Type::NullValue)
				*this = s;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return *mUnion.mString;
	}
	bool isBool() const
	{
		return mType == Type::BoolValue;
	}
	bool asBool() const
	{
		if (mType != Type::BoolValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Bool")));
		return mUnion.mBool;
	}
	bool asBool(bool b)
	{
		if (mType != Type::BoolValue) {
			if (mType == Type::NullValue)
				*this = b;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mBool;
	}
	bool isUInt() const
	{
		return mType == Type::UIntValue;
	}
	size_t asUInt() const
	{
		if (mType == Type::IntValue && mUnion.mInt >= 0) return mUnion.mInt;
		if (mType != Type::UIntValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Unsigned Int")));
		return mUnion.mUInt;
	}
	size_t asUInt(size_t s)
	{
		if (mType != Type::UIntValue) {
			if (mType == Type::NullValue)
				*this = s;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mUInt;
	}
	bool isInt() const
	{
		return mType == Type::IntValue;
	}
	int asInt() const
	{
		if (mType != Type::IntValue)
			MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Int")));
		return mUnion.mInt;
	}
	int asInt(int i)
	{
		if (mType != Type::IntValue) {
			if (mType == Type::NullValue)
				*this = i;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mInt;
	}
	bool isFloat() const
	{
		return mType == Type::FloatValue;
	}
	float asFloat() const
	{
		if (mType != Type::FloatValue
			&& mType != Type::IntValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Float")));
		if (mType == Type::IntValue) return mUnion.mInt;
		return mUnion.mFloat;
	}
	float asFloat(float f)
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
	bool isNull() const
	{
		return mType == Type::NullValue;
	}
	bool isEOL() const
	{
		return mType == Type::EndOfListValue;
	}
	const ValueType &asDefault(const ValueType &defaultValue)
	{
		if (mType != defaultValue.mType) {
			if (mType == Type::NullValue)
				*this = defaultValue;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return *this;
	}

	std::string toString() const;

	std::string getTypeString() const
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

	static ValueType EOL()
	{
		ValueType v;
		v.mType = Type::EndOfListValue;
		return v;
	}

public:


    template <class T>
    std::enable_if_t<(!std::is_class<T>::value && _isValueType<T>::value) || externValueType<T>::value, T> as() const;

    template <class T>
    std::enable_if_t<std::is_class<T>::value && _isValueType<T>::value, const T&> as() const;

	template <class T>
	std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, T> as() const {
		return static_cast<T>(as<int>());
	}

    template <class T>
	std::enable_if_t<!std::is_class<T>::value && _isValueType<T>::value, T> asDefault(T defaultValue);

    template <class T>
	std::enable_if_t<std::is_class<T>::value && _isValueType<T>::value, const T &> asDefault(const T &defaultValue);

	template <class T>
	std::enable_if_t<externValueType<T>::value, T> asDefault(const T &defaultValue);


	enum class Type : unsigned char {
		BoolValue,
		ScopeValue,
		InvPtrValue,
		IntValue,
		UIntValue,
        FloatValue,
        StringValue,
        Vector3Value,
		Vector2Value,
        NullValue,
		EndOfListValue
    } mType;

	Type type() const {
		return mType;
	}

private:
    union {
        bool mBool;
		Scripting::Scope *mScope;
        InvPtr mInvPtr;
        int mInt;
		size_t mUInt;
        float mFloat;
        std::string *mString;
		std::array<float, 3> mVector3;
		std::array<float, 2> mVector2;
    } mUnion;

	bool setType(Type t)
	{
		if (mType == Type::ScopeValue)
			decRef();
		if (mType == t) return false;
		if (mType == Type::StringValue)
			delete mUnion.mString;
		mType = t;
		return true;
	}

	void decRef();
	void incRef();

};



}

std::ostream &operator <<(std::ostream &stream,
                          const Engine::ValueType &v);


