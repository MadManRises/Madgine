#pragma once

#include "../generic/templates.h"

#include "../math/matrix3.h"
#include "../math/quaternion.h"
#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector4.h"

#include "boundapimethod.h"

#include "keyvaluevirtualiterator.h"

#include "objectptr.h"

#include "typedscopeptr.h"

#include "valuetypeexception.h"

namespace Engine {

struct MODULES_EXPORT ValueType {

    using Union = std::variant<
        std::monostate,
        std::string,
        bool,
        int,
        size_t,
        float,
        TypedScopePtr,
        Matrix3,
        Quaternion,
        Vector4,
        Vector3,
        Vector2,
        KeyValueVirtualIterator,
        BoundApiMethod,
        ApiMethod,
        ObjectPtr>;

private:
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
        Matrix3Value,
        QuaternionValue,
        Vector4Value,
        Vector3Value,
        Vector2Value,
        KeyValueVirtualIteratorValue,
        BoundApiMethodValue,
        ApiMethodValue,
        ObjectValue,

        MAX_VALUETYPE_TYPE
    };

    template <class T>
    struct isValueType {
        const constexpr static bool value = _isValueType<std::decay_t<T>>::value
            || std::is_enum<T>::value
            || (std::is_base_of_v<ScopeBase, std::decay_t<std::remove_pointer_t<std::remove_reference_t<T>>>> && std::is_pointer_v<std::remove_reference_t<T>>);
    };

    ValueType();

    ValueType(const ValueType &other);

    ValueType(ValueType &&other) noexcept;

    template <class T, class _ = std::enable_if_t<_isValueType<T>::value>>
    explicit ValueType(const T &v)
        : mUnion(v)
    {
    }

    template <class T, class _ = std::enable_if_t<std::is_enum<T>::value>>
    explicit ValueType(T val)
        : ValueType(static_cast<int>(val))
    {
    }

    explicit ValueType(const char *s);

    template <class T, class _ = std::enable_if_t<std::is_base_of<ScopeBase, T>::value && !std::is_same<ScopeBase, T>::value>>
    explicit ValueType(T *val)
        : ValueType(TypedScopePtr(val))
    {
    }

    template <class T, class _ = std::enable_if_t<std::is_base_of<ScopeBase, T>::value && !std::is_same<ScopeBase, T>::value>>
    explicit ValueType(T &val)
        : ValueType(TypedScopePtr(&val))
    {
    }

    ~ValueType();

    void clear();

    void operator=(const ValueType &other);

    template <class T, class _ = std::enable_if_t<isValueType<T>::value>>
    void operator=(T &&t)
    {
        mUnion = std::forward<T>(t);
    }

    void operator=(const char *const s);

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
    template <class T>
    bool is() const
    {
        if constexpr (_isValueType<std::decay_t<T>>::value) {
            return std::holds_alternative<std::decay_t<T>>(mUnion);
        } else if constexpr (std::is_base_of_v<ScopeBase, std::decay_t<std::remove_pointer_t<std::remove_reference_t<T>>>>) {
            static_assert(!std::is_reference_v<T>, "References are currently not supported!");
            return std::holds_alternative<TypedScopePtr>(mUnion) && std::get<TypedScopePtr>(mUnion).mType->isDerivedFrom<std::remove_pointer_t<std::remove_reference_t<T>>>();
        } else if constexpr (std::is_same_v<T, ValueType>) {
            return true;
        } else if constexpr (std::is_enum_v<T>) {
            return std::holds_alternative<int>(mUnion);
        } else {
            static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
        }
    }

    template <class T>
    decltype(auto) as() const
    {
        if constexpr (_isValueType<std::decay_t<T>>::value) {
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
        } else {
            static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
        }
    }

    template <class T>
    std::enable_if_t<std::is_enum<T>::value && !_isValueType<T>::value, T> as() const
    {
        return static_cast<T>(as<int>());
    }

    template <class T>
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
}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v);
