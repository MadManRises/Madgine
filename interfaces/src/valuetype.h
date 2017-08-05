#pragma once

#include "templates.h"
#include "scripting/datatypes/luatable.h"

#include "math/vector2.h"
#include "math/vector3.h"

namespace Engine {

	class InvScopePtr {
	public:
		InvScopePtr() = default;
		InvScopePtr(Scripting::ScopeBase* ptr) : mPtr(ptr) {}
		Scripting::ScopeBase *validate() const { return mPtr; }
		operator bool() { return mPtr != nullptr; }
		bool operator<(const InvScopePtr &other) const { return mPtr < other.mPtr; }
		bool operator==(const InvScopePtr &other) const { return mPtr == other.mPtr; }
	private:
		Scripting::ScopeBase *mPtr;
	};

	class ValueTypeException : public std::exception {
	public:
		ValueTypeException(const std::string &msg) :
			mMsg(msg)
		{
		}

		const char *what() const noexcept override {
			return mMsg.c_str();
		}

	private:
		std::string mMsg;
	};

class INTERFACES_EXPORT ValueType {
private:
	class EOLType {
	public:
		constexpr bool operator==(const EOLType &) const { return true; }
	};

	using Union = std::variant<
		std::monostate,
		std::string,
		bool,
		int,
		size_t,
		float,
		Scripting::ScopeBase *,
		InvScopePtr,
		std::array<float, 4>,
		Vector3,
		Vector2,
		EOLType,
		std::shared_ptr<Scripting::KeyValueIterator>,
		Scripting::ApiMethod,
		Scripting::LuaTable,
		Scripting::LuaThread
	>;	

	template <class T>
	struct _isValueType {
		const constexpr static bool value = variant_contains<Union, T>::value;
	};

public:

	enum class Type : unsigned char {
		NullValue,
		StringValue,
		BoolValue,
		IntValue,
		UIntValue,
		FloatValue,
		ScopeValue,
		InvScopePtrValue,		
		Vector4Value,
		Vector3Value,
		Vector2Value,		
		EndOfListValue,
		KeyValueIteratorValue,
		ApiMethodValue,
		LuaTableValue,
		LuaStateValue,

		MAX_TYPE_VALUE
	};


	template <class T>
	struct isValueType {
		const constexpr static bool value = _isValueType<std::remove_const_t<T>>::value || std::is_enum<T>::value;
	};

	ValueType()
	{

	}
	ValueType(const ValueType &other) :
		mUnion(other.mUnion)
	{
	}
	ValueType(ValueType &&other) :
		mUnion(std::forward<decltype(other.mUnion)>(other.mUnion))
	{
	}


	template <class T, class _ = std::enable_if_t<_isValueType<T>::value>>
	explicit ValueType(const T &v);

	template <class T, class _ = std::enable_if_t<std::is_enum<T>::value>>
	explicit ValueType(T val) :
		ValueType(static_cast<int>(val)) {}

	explicit ValueType(const char *s) :
		ValueType(std::string(s)) {}

	template <class T, class _ = std::enable_if_t<std::is_base_of<Scripting::ScopeBase, T>::value && !std::is_same<Scripting::ScopeBase, T>::value>>
		explicit ValueType(T *val) :
			ValueType(static_cast<Scripting::ScopeBase*>(val)) {}

	~ValueType()
	{
		clear();
	}

	static ValueType fromStack(lua_State *state, int index);
	int push(lua_State *state) const;

	void clear()
	{
		mUnion = std::monostate();
	}

    void operator=(const ValueType &other)
	{
		mUnion = other.mUnion;
	}
	template <class T, class _ = std::enable_if_t<isValueType<T>::value>>
    void operator=(const T &t)
	{
		mUnion = t;
	}
	void operator=(const char *s)
	{
		*this = std::string(s);
	}

    bool operator==(const ValueType &other) const
	{
		return mUnion == other.mUnion;
	}
	bool operator!=(const ValueType &other) const
	{
		return !(*this == other);
	}
    bool operator <(const ValueType &other) const
	{
		switch (type()) {
		case Type::StringValue:
			switch (other.type()) {
			case Type::StringValue:
				return std::get<std::string>(mUnion) < std::get<std::string>(other.mUnion);
			default:
				throw Scripting::ScriptingException(Exceptions::invalidValueType);
			}
		case Type::IntValue:
			switch (other.type()) {
			case Type::IntValue:
				return std::get<int>(mUnion) < std::get<int>(other.mUnion);
			case Type::FloatValue:
				return std::get<int>(mUnion) < std::get<float>(other.mUnion);
			default:
				throw Scripting::ScriptingException(Exceptions::invalidValueType);
			}
		default:
			throw Scripting::ScriptingException(Exceptions::invalidValueType);
		}		
	}
    bool operator >(const ValueType &other) const
	{
		return other < *this;
	}

    void operator+=(const ValueType &other)
	{
		switch (type()) {
		case Type::StringValue:
			switch (other.type()) {
			case Type::StringValue:
				std::get<std::string>(mUnion) += std::get<std::string>(other.mUnion);
				return;
			case Type::IntValue:
				std::get<std::string>(mUnion) += std::to_string(std::get<int>(other.mUnion));
				return;
			default:
				break;
			}
			break;
		case Type::IntValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<int>(mUnion) += std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				(*this) = std::get<int>(mUnion) + std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("+", getTypeString(), other.getTypeString())));
		}		
	}

	ValueType operator+(const ValueType &other) const
	{
		ValueType result = *this;
		result += other;
		return result;
	}

	void operator-=(const ValueType &other) {
		switch (type()) {
		case Type::IntValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<int>(mUnion) -= std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				(*this) = std::get<int>(mUnion) - std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("-", getTypeString(), other.getTypeString())));
		}		
	}
	ValueType operator-(const ValueType &other) const
	{
		ValueType result = *this;
		result -= other;
		return result;
	}

	void operator/=(const ValueType &other)
	{
		switch (type()) {
		case Type::IntValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<int>(mUnion) /= std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				(*this) = std::get<int>(mUnion) / std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		case Type::FloatValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<float>(mUnion) /= std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				std::get<float>(mUnion) /= std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(ValueTypeException(Exceptions::invalidTypesForOperator("/", getTypeString(), other.getTypeString())));
		}		
	}

	ValueType operator/(const ValueType &other) const
	{
		ValueType result = *this;
		result /= other;
		return result;
	}

	void operator*=(const ValueType &other)
	{
		switch (type()) {
		case Type::IntValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<int>(mUnion) *= std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				(*this) = std::get<int>(mUnion) * std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		case Type::FloatValue:
			switch (other.type()) {
			case Type::IntValue:
				std::get<float>(mUnion) *= std::get<int>(other.mUnion);
				return;
			case Type::FloatValue:
				std::get<float>(mUnion) *= std::get<float>(other.mUnion);
				return;
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(ValueTypeException(Exceptions::invalidTypesForOperator("*", getTypeString(), other.getTypeString())));
		}		
	}

	ValueType operator*(const ValueType &other) const
	{
		ValueType result = *this;
		result *= other;
		return result;
	}

	
	std::string toString() const;

	std::string getTypeString() const;
	static std::string getTypeString(Type type);
	

	static ValueType EOL()
	{
		ValueType v(EOLType{});
		return v;
	}

	bool isEOL() {
		return type() == Type::EndOfListValue;
	}

public:

	template <class T>
	std::enable_if_t<_isValueType<T>::value, bool> is() const {
		return std::holds_alternative<T>(mUnion);
	}

	template <class T>
	std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, bool> is() const {
		return is<int>();
	}
	
    template <class T>
	std::enable_if_t<_isValueType<T>::value, const T&> as() const {
		try {
			return std::get<T>(mUnion);
		}
		catch (const std::bad_variant_access &) {
			throw ValueTypeException(Exceptions::unexpectedValueType(getTypeString(), getTypeString(static_cast<Type>(variant_index<Union, T>::value))));
		}
	}

	template <class T>
	std::enable_if_t<std::is_same<T, ValueType>::value, const ValueType&> as() const {
		return *this;
	}

	template <class T>
	std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, T> as() const {
		return static_cast<T>(as<int>());
	}

    template <class T>
	std::enable_if_t<isValueType<T>::value, const T&> asDefault(const T &defaultValue) {
		if (!is<T>()) {
			mUnion = defaultValue;
		}
		return as<T>();
	}



	

	Type type() const {
		return static_cast<Type>(mUnion.index());
	}

private:

	static int apiMethodCaller(lua_State *);

	Union mUnion;

};


template <class T, class _>
ValueType::ValueType(const T &v) :
mUnion(v)
{
}

}

std::ostream &operator <<(std::ostream &stream,
                          const Engine::ValueType &v);


