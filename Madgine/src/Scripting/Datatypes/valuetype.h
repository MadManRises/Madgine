#pragma once

namespace Engine {
namespace Scripting {

enum class InvScopePtr : uintptr_t {};

class MADGINE_EXPORT ValueType {
public:
    ValueType();
    ValueType(const ValueType &other);
    ValueType(ValueType &&other);

	ValueType(const Ogre::Vector2 &v);
    ValueType(const Ogre::Vector3 &v);
    ValueType(Scope *e);
    ValueType(const std::string &s);
    ValueType(const char *s);
    ValueType(bool b);
    ValueType(int i);
    ValueType(float f);
	ValueType(InvScopePtr s);
	ValueType(size_t s);

    ~ValueType();

    void clear();

    void operator=(const ValueType &);
    void operator=(Scope *);
    void operator=(const std::string &);
    void operator=(bool);
    void operator=(int);
    void operator=(float);
    void operator=(const char *);
	void operator=(InvScopePtr);
	void operator=(size_t);

    bool operator==(const ValueType &) const;
	bool operator!=(const ValueType &) const;
    bool operator <(const ValueType &) const;
    bool operator >(const ValueType &) const;

    void operator+=(const ValueType &);
	ValueType operator+(const ValueType &) const;

	void operator-=(const ValueType &);
	ValueType operator-(const ValueType &) const;

	void operator/=(const ValueType &);
	ValueType operator/(const ValueType &) const;

	const Ogre::Vector2 &asVector2() const;
	const Ogre::Vector2 &asVector2(const Ogre::Vector2 &v);
    const Ogre::Vector3 &asVector3() const;
    const Ogre::Vector3 &asVector3(const Ogre::Vector3 &v);
    bool isScope() const;
    Scope *asScope() const;
	Scope *asScope(const Scope *s);
    bool isInvScope() const;
    InvScopePtr asInvScope() const;
	InvScopePtr asInvScope(InvScopePtr s);
    bool isString() const;
    const std::string &asString() const;
    const std::string &asString(const std::string &s);
    bool isBool() const;
    bool asBool() const;
    bool asBool(bool b);
	bool isUInt() const;
	size_t asUInt() const;
	size_t asUInt(size_t s);
    bool isInt() const;
    int asInt() const;
    int asInt(int i);
    bool isFloat() const;
    float asFloat() const;
    float asFloat(float f);
    bool isNull() const;

    std::string toString() const;

private:
	template <class T, class R>
	struct _isValueType {

	};

	template <class R>
	struct _isValueType<int, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<std::string, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<Ogre::Vector3, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<Ogre::Vector2, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<float, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<bool, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<size_t, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<InvScopePtr, R> {
		typedef R type;
	};

	template <class R>
	struct _isValueType<Scope *, R> {
		typedef R type;
	};

public:
	template <class T>
	using isValueType = typename _isValueType<std::remove_const_t<std::remove_reference_t<T>>, T>::type;

    template <class T>
    std::enable_if_t<!std::is_class<T>::value, isValueType<T>> as() const;

    template <class T>
    std::enable_if_t<std::is_class<T>::value, isValueType<const T&>> as() const;

    template <class T>
    std::enable_if_t<!std::is_class<T>::value, isValueType<T>> asDefault(T defaultValue);

    template <class T>
    std::enable_if_t<std::is_class<T>::value, isValueType<const T &>> asDefault(const T &defaultValue);

private:
	enum class Type : unsigned char {
		BoolValue,
		ScopeValue,
		InvScopeValue,
		IntValue,
		UIntValue,
        FloatValue,
        StringValue,
        Vector3Value,
		Vector2Value,
        NullValue
    } mType;

    union {
        bool mBool;
        Scope *mScope;
        InvScopePtr mInvScope;
        int mInt;
		size_t mUInt;
        float mFloat;
        std::string *mString;
        Ogre::Vector3 *mVector3;
		Ogre::Vector2 *mVector2;
    } mUnion;

    bool setType(Type t);

    void decRef();
    void incRef();

    friend class Serialize::SerializeInStream;
    friend class Serialize::SerializeOutStream;

};


}
}

std::ostream &operator <<(std::ostream &stream,
                          const Engine::Scripting::ValueType &v);


