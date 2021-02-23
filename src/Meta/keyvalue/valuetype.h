#pragma once

#include "../math/matrix3.h"
#include "../math/matrix4.h"
#include "../math/quaternion.h"
#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector4.h"

#include "apifunction.h"

#include "keyvaluevirtualiterator.h"

#include "objectptr.h"

#include "typedscopeptr.h"

#include "valuetypeexception.h"

#include "ownedscopeptr.h"

#include "Generic/cow.h"
#include "Generic/cowstring.h"

#include "valuetype_desc.h"
#include "valuetype_forward.h"

namespace Engine {

struct META_EXPORT ValueType {

    using Union = std::variant<
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, ...) Storage
#include "valuetypedefinclude.h"
        >;    

    
    using NonDefaultConstructibleTypes = Engine::type_pack<Engine::KeyValueVirtualRange>;

    ValueType();

    ValueType(const ValueType &other);

    ValueType(ValueType &&other) noexcept;

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, ValueType>>>
    explicit ValueType(T &&v)
        : mUnion(std::in_place_index<static_cast<size_t>(static_cast<ValueTypeEnum>(toValueTypeIndex<std::decay_t<T>>()))>, std::forward<T>(v))
    {
    }

    template <typename T, typename _ = std::enable_if_t<std::is_enum_v<T>>>
    explicit ValueType(T val)
        : ValueType(static_cast<int>(val))
    {
    }

    template <typename T>
    explicit ValueType(T *val)
        : ValueType(TypedScopePtr(val))
    {
    }

    ~ValueType();

    void clear();

    void operator=(const ValueType &other);
    void operator=(ValueType &&other);

    template <typename T, typename _ = std::enable_if_t<!std::is_same_v<std::decay_t<T>, ValueType>>>
    void operator=(T &&t)
    {
        constexpr size_t index = static_cast<size_t>(static_cast<ValueTypeEnum>(toValueTypeIndex<std::decay_t<T>>()));
        if (mUnion.index() == index)
            std::get<index>(mUnion) = std::forward<T>(t);
        else
            mUnion.emplace<index>(std::forward<T>(t));
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

    template <typename T>
    bool is() const;

    template <typename T>
    ValueType_Return<T> as() const;

    template <typename T>
    ValueType_Return<T> asDefault(const T &defaultValue)
    {
        if (!is<T>()) {
            mUnion = defaultValue;
        }
        return as<T>();
    }

    bool isReference() const;

    ValueTypeIndex index() const;
    ValueTypeDesc type() const;
    
    void setType(ValueTypeDesc type);

private:
    Union mUnion;
};

struct META_EXPORT ValueTypeRef {

private:
    template <typename T>
    void *toPtrHelper(T &&val)
    {
        if constexpr (!std::is_reference_v<T> || std::is_const_v<std::remove_reference_t<T>>) {
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
        , mData(toPtrHelper(convert_ValueType<false>(std::forward<T>(val))))
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
    return toValueTypeDesc<T>().canAccept(type());
}

template <typename T>
ValueType_Return<T> ValueType::as() const
{

    if constexpr (isValueTypePrimitive_v<T>) {
        return std::get<static_cast<size_t>(toValueTypeIndex<T>().mIndex)>(mUnion);
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return *this;
    } else if constexpr (isScopeRef_v<T>) {
        if constexpr (std::is_pointer_v<T>) {
            return std::get<TypedScopePtr>(mUnion).safe_cast<std::remove_pointer_t<T>>();
        } else {
            return std::get<OwnedScopePtr>(mUnion).safe_cast<T>();
        }
    } else if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(std::get<int>(mUnion));
    } else {
        static_assert(dependent_bool<T, false>::value, "Invalid target type for Valuetype cast provided!");
    }
}

}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v);
