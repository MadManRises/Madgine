#pragma once

#include "templates.h"

#include "../scripting/datatypes/luatable.h"

#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector4.h"

#include "invscopeptr.h"

#include "valuetypeexception.h"

#include "../scripting/datatypes/luathread.h"
#include "../scripting/types/apihelper.h"

#include "../math/matrix3.h"

namespace Engine
{


	class INTERFACES_EXPORT ValueType
	{
	private:

		using Union = std::variant<
			std::monostate,
			std::string,
			bool,
			int,
			size_t,
			float,
			Scripting::ScopeBase *,
			InvScopePtr,
			Matrix3,
			Vector4,
			Vector3,
			Vector2,
			std::shared_ptr<KeyValueIterator>,
			Scripting::ApiMethod,
			Scripting::LuaTable,
			Scripting::LuaThread
		>;

		template <class T>
		struct _isValueType
		{
			const constexpr static bool value = variant_contains<Union, T>::value;
		};

	public:

		enum class Type : unsigned char
		{
			NullValue,
			StringValue,
			BoolValue,
			IntValue,
			UIntValue,
			FloatValue,
			ScopeValue,
			InvScopePtrValue,
			Matrix3Value,
			Vector4Value,
			Vector3Value,
			Vector2Value,
			KeyValueIteratorValue,
			ApiMethodValue,
			LuaTableValue,
			LuaStateValue,

			MAX_VALUETYPE_TYPE
		};


		template <class T>
		struct isValueType
		{
			const constexpr static bool value = _isValueType<std::decay_t<T>>::value || std::is_enum<T>::value;
		};

		ValueType()
		{
		}

		ValueType(const ValueType& other) :
			mUnion(other.mUnion)
		{
		}

		ValueType(ValueType&& other) noexcept :
			mUnion(std::forward<decltype(other.mUnion)>(other.mUnion))
		{
		}


		template <class T, class _ = std::enable_if_t<_isValueType<T>::value>>
		explicit ValueType(const T& v) :
			mUnion(v)
		{
		}

		template <class T, class _ = std::enable_if_t<std::is_enum<T>::value>>
		explicit ValueType(T val) :
			ValueType(static_cast<int>(val))
		{
		}

		explicit ValueType(const char* s) :
			ValueType(std::string(s))
		{
		}

		template <class T, class _ = std::enable_if_t<std::is_base_of<Scripting::ScopeBase, T>::value && !std::is_same<
			          Scripting::ScopeBase, T>::value>>
		explicit ValueType(T* val) :
			ValueType(static_cast<Scripting::ScopeBase*>(val))
		{
		}

		~ValueType()
		{
			clear();
		}

		static ValueType fromStack(lua_State* state, int index);
		int push(lua_State* state) const;

		void clear()
		{
			mUnion = std::monostate();
		}

		void operator=(const ValueType& other)
		{
			mUnion = other.mUnion;
		}

		template <class T, class _ = std::enable_if_t<isValueType<T>::value>>
		void operator=(T&& t)
		{
			mUnion = std::forward<T>(t);
		}

		void operator=(const char* const s)
		{
			mUnion = std::string(s);
		}

		bool operator==(const ValueType& other) const
		{
			return mUnion == other.mUnion;
		}

		bool operator!=(const ValueType& other) const
		{
			return !(*this == other);
		}

		bool operator <(const ValueType& other) const
		{
			switch (type())
			{
			case Type::StringValue:
				switch (other.type())
				{
				case Type::StringValue:
					return std::get<std::string>(mUnion) < std::get<std::string>(other.mUnion);
				default:
					throw Scripting::ScriptingException(Database::Exceptions::invalidValueType);
				}
			case Type::IntValue:
				switch (other.type())
				{
				case Type::IntValue:
					return std::get<int>(mUnion) < std::get<int>(other.mUnion);
				case Type::FloatValue:
					return std::get<int>(mUnion) < std::get<float>(other.mUnion);
				default:
					throw Scripting::ScriptingException(Database::Exceptions::invalidValueType);
				}
			default:
				throw Scripting::ScriptingException(Database::Exceptions::invalidValueType);
			}
		}

		bool operator >(const ValueType& other) const
		{
			return other < *this;
		}

		void operator+=(const ValueType& other)
		{
			switch (type())
			{
			case Type::StringValue:
				switch (other.type())
				{
				case Type::StringValue:
					std::get<std::string>(mUnion) += std::get<std::string>(other.mUnion);
					return;
				case Type::IntValue:
					std::get<std::string>(mUnion) += std::to_string(std::get<int>(other.mUnion));
				default:
					break;
				}
				break;
			case Type::IntValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<int>(mUnion) += std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					*this = std::get<int>(mUnion) + std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			default:
				throw Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("+", getTypeString(), other
					.getTypeString()));
			}
		}

		ValueType operator+(const ValueType& other) const
		{
			ValueType result = *this;
			result += other;
			return result;
		}

		void operator-=(const ValueType& other)
		{
			switch (type())
			{
			case Type::IntValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<int>(mUnion) -= std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					*this = std::get<int>(mUnion) - std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			default:
				throw Scripting::ScriptingException(Database::Exceptions::invalidTypesForOperator("-", getTypeString(), other
					.getTypeString()));
			}
		}

		ValueType operator-(const ValueType& other) const
		{
			ValueType result = *this;
			result -= other;
			return result;
		}

		void operator/=(const ValueType& other)
		{
			switch (type())
			{
			case Type::IntValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<int>(mUnion) /= std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					*this = std::get<int>(mUnion) / std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			case Type::FloatValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<float>(mUnion) /= std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					std::get<float>(mUnion) /= std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			default:
				throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("/", getTypeString(), other.
					getTypeString()));
			}
		}

		ValueType operator/(const ValueType& other) const
		{
			ValueType result = *this;
			result /= other;
			return result;
		}

		void operator*=(const ValueType& other)
		{
			switch (type())
			{
			case Type::IntValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<int>(mUnion) *= std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					*this = std::get<int>(mUnion) * std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			case Type::FloatValue:
				switch (other.type())
				{
				case Type::IntValue:
					std::get<float>(mUnion) *= std::get<int>(other.mUnion);
					return;
				case Type::FloatValue:
					std::get<float>(mUnion) *= std::get<float>(other.mUnion);
				default:
					break;
				}
				break;
			default:
				throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("*", getTypeString(), other.
					getTypeString()));
			}
		}

		ValueType operator*(const ValueType& other) const
		{
			ValueType result = *this;
			result *= other;
			return result;
		}


		std::string toString() const;

		std::string getTypeString() const;
		static std::string getTypeString(Type type);

		template <typename V>
		void visit(V &&visitor) const
		{
			std::visit(std::forward<V>(visitor), mUnion);
		}

	public:

		template <class T>
		std::enable_if_t<_isValueType<T>::value, bool> is() const
		{
			return std::holds_alternative<T>(mUnion);
		}

		template <class T>
		std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, bool> is() const
		{
			return is<int>();
		}

		template <class T>
		std::enable_if_t<_isValueType<T>::value, const T&> as() const
		{
			try
			{
				return std::get<T>(mUnion);
			}
			catch (const std::bad_variant_access&)
			{
				throw ValueTypeException(Database::Exceptions::unexpectedValueType(getTypeString(),
				                                                         getTypeString(
					                                                         static_cast<Type>(variant_index<Union, T>::value))));
			}
		}

		template <class T>
		std::enable_if_t<std::is_same<T, ValueType>::value, const ValueType&> as() const
		{
			return *this;
		}

		template <class T>
		std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, T> as() const
		{
			return static_cast<T>(as<int>());
		}

		template <class T>
		std::enable_if_t<isValueType<T>::value, const T&> asDefault(const T& defaultValue)
		{
			if (!is<T>())
			{
				mUnion = defaultValue;
			}
			return as<T>();
		}


		Type type() const
		{
			return static_cast<Type>(mUnion.index());
		}

	private:

		Scripting::APIHelper::Userdata *pushUserdata(lua_State* state, const Scripting::APIHelper::Userdata &data) const;

		static int apiMethodCaller(lua_State*);

		Union mUnion;
	};


}

std::ostream& operator <<(std::ostream& stream,
                          const Engine::ValueType& v);
