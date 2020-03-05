#pragma once

#include "../generic/templates.h"

#include "../math/matrix3.h"
#include "../math/matrix4.h"
#include "../math/quaternion.h"
#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector4.h"

#include "apimethod.h"

#include "keyvaluevirtualiterator.h"

#include "objectptr.h"

#include "typedscopeptr.h"

#include "valuetypeexception.h"

#include "../generic/cow.h"
#include "../generic/heapobject.h"

namespace Engine {

struct MODULES_EXPORT ValueType {

    using Union = std::variant<
        std::monostate,
        HeapObject<std::string>,
        std::string_view,
        bool,
        int,
        size_t,
        float,
        TypedScopePtr,
        CoW<Matrix3>,
        CoW<Matrix4>,
        Quaternion,
        Vector4,
        Vector3,
        Vector2,
        KeyValueVirtualIterator,
        ApiMethod,
        ObjectPtr>;

private:
    template <typename T>
    using _isValueType = std::bool_constant<variant_contains_v<Union, T> || std::is_same_v<T, std::string>>;

public:
    enum class Type : unsigned char {
        NullValue,
        OwningStringValue,
        NonOwningStringValue,
        BoolValue,
        IntValue,
        UIntValue,
        FloatValue,
        ScopeValue,
        Matrix3Value,
        Matrix4Value,
        QuaternionValue,
        Vector4Value,
        Vector3Value,
        Vector2Value,
        KeyValueVirtualIteratorValue,
        ApiMethodValue,
        ObjectValue,

        MAX_VALUETYPE_TYPE
    };

    template <typename T>
    struct isValueType {
        const constexpr static bool value = _isValueType<std::decay_t<T>>::value
            || std::is_enum<T>::value
            || (std::is_same_v<std::decay_t<T>, Matrix3> || std::is_same_v<std::decay_t<T>, Matrix4>)
            || (std::is_base_of_v<ScopeBase, std::decay_t<std::remove_pointer_t<std::remove_reference_t<T>>>> && std::is_pointer_v<std::remove_reference_t<T>>);
    };

    ValueType();

    ValueType(const ValueType &other);

    ValueType(ValueType &&other) noexcept;

    template <typename T, typename _ = std::enable_if_t<_isValueType<T>::value>>
    explicit ValueType(const T &v)
        : mUnion(v)
    {
    }

	explicit ValueType(const char *) = delete;
    explicit ValueType(std::string &&s);
    explicit ValueType(const std::string &s);
    explicit ValueType(const std::string &&s);

    explicit ValueType(Matrix3 &&m);
    explicit ValueType(const Matrix3 &m);
    explicit ValueType(const Matrix3 &&m);
    explicit ValueType(Matrix4 &&m);
    explicit ValueType(const Matrix4 &m);
    explicit ValueType(const Matrix4 &&m);

    template <typename T, typename _ = std::enable_if_t<std::is_enum<T>::value>>
    explicit ValueType(T val)
        : ValueType(static_cast<int>(val))
    {
    }

    //ValueType(const char *s) = delete;

    template <typename T, typename _ = std::enable_if_t<std::is_base_of<ScopeBase, T>::value && !std::is_same<ScopeBase, T>::value>>
    explicit ValueType(T *val)
        : ValueType(TypedScopePtr(val))
    {
    }

    template <typename T, typename _ = std::enable_if_t<std::is_base_of<ScopeBase, T>::value && !std::is_same<ScopeBase, T>::value>>
    explicit ValueType(T &val)
        : ValueType(TypedScopePtr(&val))
    {
    }

    ~ValueType();

    void clear();

    void operator=(const ValueType &other);

	void operator=(const char *) = delete;
    void operator=(std::string &&s);
    void operator=(std::string &s);
    void operator=(const std::string &s);
    void operator=(const std::string &&s);

    template <typename T, typename _ = std::enable_if_t<isValueType<T>::value>>
    void operator=(T &&t)
    {
        mUnion = std::forward<T>(t);
    }

    bool operator==(const ValueType &other) const;

    bool operator!=(const ValueType &other) const;

    bool operator<(const ValueType &other) const;

    bool operator>(const ValueType &other) const;

    void operator+=(const ValueType &other);

    ValueType operator+(const ValueType &other) const;

    void operator-=(const ValueType &other);

    ValueType operator-(const ValueType &other) const;

    void operator/=(const ValueType &other);

    ValueType operator/(const ValueType &other) const;

    void operator*=(const ValueType &other);

    ValueType operator*(const ValueType &other) const;

    std::string toString() const;
    std::string toShortString() const;

    std::string getTypeString() const;
    static std::string getTypeString(Type type);

    template <typename V>
    decltype(auto) visit(V &&visitor) const &
    {
        return std::visit(std::forward<V>(visitor), mUnion);
    }

    template <typename V>
    decltype(auto) visit(V &&visitor) &
    {
        return std::visit(std::forward<V>(visitor), mUnion);
    }

    template <typename V>
    decltype(auto) visit(V &&visitor) const &&
    {
        return std::visit(std::forward<V>(visitor), std::move(mUnion));
    }

    template <typename V>
    decltype(auto) visit(V &&visitor) &&
    {
        return std::visit(std::forward<V>(visitor), std::move(mUnion));
    }

public:
    template <typename T>
    bool is() const;

    template <typename T>
    decltype(auto) as() const;

    template <typename T>
    std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, T> as() const;

    template <typename T>
    std::enable_if_t<isValueType<T>::value, const T &> asDefault(const T &defaultValue)
    {
        if (!is<T>()) {
            mUnion = defaultValue;
        }
        return as<T>();
    }

    Type type() const;

private:
    Union mUnion;
};

struct MODULES_EXPORT ValueTypeRef {

private:
    template <typename T>
    void *toPtrHelper(T &val)
    {
        if constexpr (std::is_const_v<T> || std::is_base_of<ScopeBase, T>::value) {
            return nullptr;
        } else {
            return &val;
        }
    }

public:
    template <typename T, typename _ = std::enable_if_t<ValueType::isValueType<T>::value || (std::is_base_of<ScopeBase, T>::value && !std::is_same<ScopeBase, T>::value)>>
    explicit ValueTypeRef(T &val)
        : mValue(val)
        , mData(toPtrHelper(val))
    {
    }

    ValueTypeRef(const ValueTypeRef &) = delete;
    ValueTypeRef(ValueTypeRef &&other);

    ValueTypeRef &operator=(const ValueTypeRef &) = delete;

    const ValueType &value() const;

    operator const ValueType &() const;

    bool isEditable() const;

    ValueTypeRef &operator=(const ValueType &v);

private:
    ValueType mValue;
    void *mData = nullptr;
};

template <typename T>
bool ValueType::is() const
{
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        return std::holds_alternative<std::string_view>(mUnion) || std::holds_alternative<HeapObject<std::string>>(mUnion);
    } else if constexpr (_isValueType<std::decay_t<T>>::value) {
        return std::holds_alternative<std::decay_t<T>>(mUnion);
    } else if constexpr (std::is_base_of_v<ScopeBase, std::decay_t<std::remove_pointer_t<std::remove_reference_t<T>>>>) {
        static_assert(!std::is_reference_v<T>, "References are currently not supported!");
        return std::holds_alternative<TypedScopePtr>(mUnion) && std::get<TypedScopePtr>(mUnion).mType->isDerivedFrom<std::remove_pointer_t<std::remove_reference_t<T>>>();
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return true;
    } else if constexpr (std::is_enum_v<T>) {
        return std::holds_alternative<int>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix3>) {
        return std::holds_alternative<CoW<Matrix3>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix4>) {
        return std::holds_alternative<CoW<Matrix4>>(mUnion);
    } else {
        static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
    }
}

template <typename T>
decltype(auto) ValueType::as() const
{
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        return std::holds_alternative<std::string_view>(mUnion) ? T { std::get<std::string_view>(mUnion) } : T { std::get<HeapObject<std::string>>(mUnion) };
    } else if constexpr (_isValueType<std::decay_t<T>>::value) {
        if (std::holds_alternative<std::decay_t<T>>(mUnion))
            return std::get<std::decay_t<T>>(mUnion);
        else
            throw ValueTypeException(Database::Exceptions::unexpectedValueType(getTypeString(),
                getTypeString(
                    static_cast<Type>(variant_index<Union, std::decay_t<T>>::value))));
    } else if constexpr (std::is_base_of_v<ScopeBase, std::decay_t<std::remove_pointer_t<std::remove_reference_t<T>>>>) {
        if constexpr (std::is_pointer_v<T>) {
            return std::get<TypedScopePtr>(mUnion).safe_cast<std::remove_pointer_t<T>>();
        } else {
            static_assert(dependent_bool<T, false>::value, "References are currently not supported!");
            return *std::get<TypedScopePtr>(mUnion).safe_cast<std::remove_reference_t<T>>();
        }
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return *this;
    } else if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(std::get<int>(mUnion));
    } else if constexpr (std::is_same_v<T, Matrix3>) {
        return std::get<CoW<Matrix3>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix4>) {
        return std::get<CoW<Matrix4>>(mUnion);
    } else {
        static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
    }
}

template <typename T>
std::enable_if_t<std::is_enum<T>::value && !ValueType::_isValueType<T>::value, T> ValueType::as() const
{
    return static_cast<T>(as<int>());
}

}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v);
