#pragma once





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
	struct _isValueType<std::array<float, 4>> {
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
	struct _isValueType<InvScopePtr> {
		const constexpr static bool value = true;
	};

	template <>
	struct _isValueType<Scripting::ScopeBase *> {
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
	}
	ValueType(ValueType &&other) :
		mType(other.mType),
		mUnion(other.mUnion)
	{
		other.mType = Type::NullValue;
	}

	explicit ValueType(Scripting::ScopeBase *e) :
		mType(Type::ScopeValue)
	{
		mUnion.mScope = e;
	}
	explicit ValueType(const std::string &s) :
		mType(Type::StringValue)
	{
		mUnion.mString = new std::string(s);
	}
	explicit ValueType(const char *s) :
		mType(Type::StringValue)
	{
		mUnion.mString = new std::string(s);
	}
	explicit ValueType(bool b) :
		mType(Type::BoolValue)
	{
		mUnion.mBool = b;
	}
	explicit ValueType(int i) :
		mType(Type::IntValue)
	{
		mUnion.mInt = i;
	}
	explicit ValueType(float f) :
		mType(Type::FloatValue)
	{
		mUnion.mFloat = f;
	}
	explicit ValueType(const std::array<float, 4> &v) :
		mType(Type::Vector4Value)
	{
		mUnion.mVector4 = v;
	}
	explicit ValueType(const std::array<float, 3> &v) :
		mType(Type::Vector3Value)
	{
		mUnion.mVector3 = v;
	}
	explicit ValueType(const std::array<float, 2> &v) :
		mType(Type::Vector2Value)
	{
		mUnion.mVector2 = v;
	}
	explicit ValueType(InvScopePtr s) :
		mType(Type::InvScopePtrValue)
	{
		mUnion.mInvPtr = s;
	}

	template <class T, class _ = std::enable_if_t<externValueType<T>::value>>
	explicit ValueType(const T &);

	template <class T, class _ = std::enable_if_t<std::is_enum<T>::value>>
	explicit ValueType(T val) :
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
	}
    void operator=(Scripting::ScopeBase *s)
	{
		setType(Type::ScopeValue);
		mUnion.mScope = s;
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
	void operator=(const std::array<float, 4> &v)
	{
		setType(Type::Vector4Value);
		mUnion.mVector4 = v;
	}
	void operator=(const char *s)
	{
		*this = std::string(s);
	}
	void operator=(InvScopePtr p)
	{
		setType(Type::InvScopePtrValue);
		mUnion.mInvPtr = p;
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
		case Type::Vector4Value:
			return mUnion.mVector4 == other.mUnion.mVector4;
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
			switch (other.mType) {
			case Type::StringValue:
				return *mUnion.mString < *other.mUnion.mString;
			default:
				throw Scripting::ScriptingException(Exceptions::invalidValueType);
			}
		case Type::IntValue:
			switch (other.mType) {
			case Type::IntValue:
				return mUnion.mInt < other.mUnion.mInt;
			case Type::FloatValue:
				return mUnion.mInt < other.mUnion.mFloat;
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
		switch (mType) {
		case Type::StringValue:
			switch (other.mType) {
			case Type::StringValue:
				*mUnion.mString += *other.mUnion.mString;
				return;
			case Type::IntValue:
				*mUnion.mString += std::to_string(other.mUnion.mInt);
				return;
			default:
				break;
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
		switch (mType) {
		case Type::IntValue:
			switch (other.mType) {
			case Type::IntValue:
				mUnion.mInt -= other.mUnion.mInt;
				return;
			case Type::FloatValue:
				(*this) = mUnion.mInt - other.mUnion.mFloat;
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
		switch (mType) {
		case Type::IntValue:
			switch (other.mType) {
			case Type::IntValue:
				mUnion.mInt /= other.mUnion.mInt;
				return;
			case Type::FloatValue:
				(*this) = mUnion.mInt / other.mUnion.mFloat;
				return;
			default:
				break;
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
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("/", getTypeString(), other.getTypeString())));
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
		switch (mType) {
		case Type::IntValue:
			switch (other.mType) {
			case Type::IntValue:
				mUnion.mInt *= other.mUnion.mInt;
				return;
			case Type::FloatValue:
				(*this) = mUnion.mInt * other.mUnion.mFloat;
				return;
			default:
				break;
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
			default:
				break;
			}
			break;
		default:
			MADGINE_THROW_NO_TRACE(Scripting::ScriptingException(Exceptions::invalidTypesForOperator("*", getTypeString(), other.getTypeString())));
		}		
	}

	ValueType operator*(const ValueType &other) const
	{
		ValueType result = *this;
		result *= other;
		return result;
	}

	bool isVector2() const
	{
		return mType == Type::Vector2Value;
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


	bool isVector3() const
	{
		return mType == Type::Vector3Value;
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


	bool isVector4() const
	{
		return mType == Type::Vector4Value;
	}
	const std::array<float, 4> &asVector4() const
	{
		if (mType != Type::Vector4Value) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Vector4")));
		return mUnion.mVector4;
	}
	const std::array<float, 4> &asVector4(const std::array<float, 4> &v)
	{
		if (mType != Type::Vector4Value) {
			if (mType == Type::NullValue)
				*this = v;
			else
				MADGINE_THROW(Scripting::ScriptingException("Can't assign default value to non-Null variable!"));
		}
		return mUnion.mVector4;
	}
	bool isScope() const
	{
		return mType == Type::ScopeValue;
	}
	Scripting::ScopeBase *asScope() const
	{
		if (mType != Type::ScopeValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("Scope")));
		return mUnion.mScope;
	}
	Scripting::ScopeBase *asScope(Scripting::ScopeBase *s)
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
	bool isInvPtr() const
	{
		return mType == Type::InvScopePtrValue;
	}
	const InvScopePtr &asInvPtr() const
	{
		if (mType != Type::InvScopePtrValue) MADGINE_THROW(Scripting::ScriptingException(Exceptions::notValueType("InvScope")));
		return mUnion.mInvPtr;
	}
	const InvScopePtr &asInvPtr(InvScopePtr s)
	{
		if (mType != Type::InvScopePtrValue) {
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
		if (mType == Type::IntValue) return static_cast<float>(mUnion.mInt);
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
		if (mType == Type::IntValue) return static_cast<float>(mUnion.mInt);
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
		case Type::InvScopePtrValue:
			return "Invalid Scope";
		case Type::NullValue:
			return "Null-Type";
		case Type::ScopeValue:
			return "Scope";
		case Type::StringValue:
			return "String";
		case Type::Vector2Value:
			return "Vector2";
		case Type::Vector3Value:
			return "Vector3";
		case Type::Vector4Value:
			return "Vector4";
		default:
			throw 0;
		}
	}

	static ValueType EOL()
	{
		ValueType v;
		v.mType = Type::EndOfListValue;
		return v;
	}

public:

	template <class T>
	std::enable_if_t<_isValueType<T>::value, bool> is() const;

	template <class T>
	std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, bool> is() const {
		return isInt();
	}
	

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
		InvScopePtrValue,
		IntValue,
        FloatValue,
        StringValue,
		Vector4Value,
        Vector3Value,
		Vector2Value,
        NullValue,
		EndOfListValue,
		MAX_TYPE_VALUE
    } mType;

	Type type() const {
		return mType;
	}

private:
    union {
        bool mBool;
		Scripting::ScopeBase *mScope;
        InvScopePtr mInvPtr;
        int mInt;
        float mFloat;
        std::string *mString;
		std::array<float, 4> mVector4;
		std::array<float, 3> mVector3;
		std::array<float, 2> mVector2;
    } mUnion;

	bool setType(Type t)
	{
		if (mType == t) return false;
		if (mType == Type::StringValue)
			delete mUnion.mString;
		mType = t;
		return true;
	}

};



}

std::ostream &operator <<(std::ostream &stream,
                          const Engine::ValueType &v);


