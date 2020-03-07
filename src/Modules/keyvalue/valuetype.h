#pragma once

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

#include "valuetype_forward.h"

namespace Engine {

struct MODULES_EXPORT ValueType {

    using Union = std::variant<
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Type, Storage, Name) Storage
#include "valuetypedef.h"
        >;

    enum class Type : unsigned char {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Type, Storage, Name) Name##Value
#include "valuetypedef.h"
        ,
        MAX_VALUETYPE_TYPE
    };

    ValueType();

    ValueType(const ValueType &other);

    ValueType(ValueType &&other) noexcept;

    template <typename T, typename _ = std::enable_if_t<isValueTypePrimitive_v<T>>>
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
    void operator=(ValueType &&other);

    void operator=(const char *) = delete;
    void operator=(std::string &&s);
    void operator=(std::string &s);
    void operator=(const std::string &s);
    void operator=(const std::string &&s);

    template <typename T, typename _ = std::enable_if_t<isValueType_v<std::remove_reference_t<T>>>>
    void operator=(T &&t)
    {
        *this = ValueType { std::forward<T>(t) };
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
    ValueType_Return<T> as() const;

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, T> as() const;

    template <typename T>
    ValueType_Return<T> asDefault(const T &defaultValue)
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
    void *toPtrHelper(T &&val)
    {
        if constexpr (!std::is_reference_v<T> || std::is_const_v<std::remove_reference_t<T>> ) {
            return nullptr;
        } else {
            return &val;
        }
    }

public:
    ValueTypeRef() = default;

    template <typename T, typename _ = std::enable_if_t<isValueType_v<T>>>
    explicit ValueTypeRef(T &&val)
        : mValue(std::forward<T>(val))
        , mData(toPtrHelper(convert_ValueType(std::forward<T>(val))))
    {
    }

    ValueTypeRef(const ValueTypeRef &) = delete;
    ValueTypeRef(ValueTypeRef &&other);

    ValueTypeRef &operator=(const ValueTypeRef &) = delete;
    ValueTypeRef &operator=(ValueTypeRef &&);

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
    if constexpr (std::is_same_v<T, std::string>) {
        return std::holds_alternative<HeapObject<std::string>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix3>) {
        return std::holds_alternative<CoW<Matrix3>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix4>) {
        return std::holds_alternative<CoW<Matrix4>>(mUnion);
    } else if constexpr (isValueTypePrimitive<T>::value) {
        if constexpr (std::is_same_v<T, std::string_view>) {
            if (std::holds_alternative<HeapObject<std::string>>(mUnion)) {
                return true;
            }
        }
        return std::holds_alternative<T>(mUnion);
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return true;
    } else if constexpr (isScopeRef_v<T>) {
        static_assert(!std::is_reference_v<T>, "References are currently not supported!");
        return std::holds_alternative<TypedScopePtr>(mUnion) && std::get<TypedScopePtr>(mUnion).mType->isDerivedFrom<std::remove_pointer_t<T>>();
    } else if constexpr (std::is_enum_v<T>) {
        return std::holds_alternative<int>(mUnion);
    } else {
        static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
    }
}

template <typename T>
ValueType_Return<T> ValueType::as() const
{
    if constexpr (std::is_same_v<T, std::string>) {
        return std::get<HeapObject<std::string>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix3>) {
        return std::get<CoW<Matrix3>>(mUnion);
    } else if constexpr (std::is_same_v<T, Matrix4>) {
        return std::get<CoW<Matrix4>>(mUnion);
    } else if constexpr (isValueTypePrimitive_v<T>) {
        if constexpr (std::is_same_v<T, std::string_view>) {
            if (std::holds_alternative<HeapObject<std::string>>(mUnion)) {
                return static_cast<const std::string&>(std::get<HeapObject<std::string>>(mUnion));
            }
        }
        return std::get<T>(mUnion);
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return *this;
    } else if constexpr (isScopeRef_v<T>) {
        if constexpr (std::is_pointer_v<T>) {
            return std::get<TypedScopePtr>(mUnion).safe_cast<std::remove_pointer_t<T>>();
        } else {
            static_assert(dependent_bool<T, false>::value, "References are currently not supported!");
            return *std::get<TypedScopePtr>(mUnion).safe_cast<std::remove_reference_t<T>>();
        }
    } else if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(std::get<int>(mUnion));
    } else {
        static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
    }
}

template <typename T>
std::enable_if_t<std::is_enum_v<T>, T> ValueType::as() const
{
    return static_cast<T>(as<int>());
}

}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v);
