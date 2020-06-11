#pragma once

#include "valuetype_types.h"

#include "metatable.h"

namespace Engine {

enum class ValueTypeEnum : unsigned char {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, ...) Name##Value
#include "valuetypedef.h"
    ,
    MAX_VALUETYPE_TYPE
};

struct MODULES_EXPORT ValueTypeIndex {
    ValueTypeEnum mIndex;

    constexpr ValueTypeIndex(ValueTypeEnum index)
        : mIndex(index)
    {
    }

    constexpr operator ValueTypeEnum() const
    {
        return mIndex;
    }

    std::string_view toString() const;
};

enum class ExtendedValueTypeEnum : unsigned char {
    VariadicListType = static_cast<unsigned char>(ValueTypeEnum::MAX_VALUETYPE_TYPE),
    VariadicPassthroughListType,
    GenericType
};

struct MODULES_EXPORT ExtendedValueTypeIndex {
    union {
        ValueTypeIndex mType;
        ExtendedValueTypeEnum mExtendedType;
    };

    constexpr ExtendedValueTypeIndex(ValueTypeIndex t)
        : mType(t)
    {
    }

    constexpr ExtendedValueTypeIndex(ExtendedValueTypeEnum t)
        : mExtendedType(t)
    {
    }

    constexpr bool canAccept(ValueTypeIndex valueType)
    {
        return mExtendedType == ExtendedValueTypeEnum::GenericType || mType == valueType;
    }

    constexpr bool isRegular() const
    {
        return mType < ValueTypeEnum::MAX_VALUETYPE_TYPE;
    }

    constexpr operator ExtendedValueTypeEnum() const
    {
        return mExtendedType;
    }

    constexpr operator ValueTypeIndex() const
    {
        assert(isRegular());
        return mType;
    }

    constexpr bool operator==(const ExtendedValueTypeIndex &other) const
    {
        return mType == other.mType;
    }

    constexpr bool operator!=(const ExtendedValueTypeIndex &other) const
    {
        return mType != other.mType;
    }

    constexpr bool operator==(const ValueTypeIndex &other) const
    {
        return mType == other;
    }

    constexpr bool operator!=(const ValueTypeIndex &other) const
    {
        return mType != other;
    }

    constexpr bool operator==(const ExtendedValueTypeEnum &other) const
    {
        return mExtendedType == other;
    }

    constexpr bool operator!=(const ExtendedValueTypeEnum &other) const
    {
        return mExtendedType != other;
    }

    std::string_view toString() const;
};

union ValueTypeSecondaryTypeInfo {

    constexpr ValueTypeSecondaryTypeInfo()
        : mDummy(nullptr)
    {
    }
    constexpr ValueTypeSecondaryTypeInfo(const MetaTable **metaTable)
        : mMetaTable(metaTable)
    {
    }

    bool operator==(const ValueTypeSecondaryTypeInfo &other) const
    {
        return mDummy == other.mDummy;
    }

    bool operator!=(const ValueTypeSecondaryTypeInfo &other) const
    {
        return mDummy == other.mDummy;
    }

    const void *mDummy;
    const MetaTable **mMetaTable;
    const FunctionTable **mFunctionTable;
};

struct MODULES_EXPORT ValueTypeDesc {
    ValueTypeIndex mType;
    ValueTypeSecondaryTypeInfo mSecondary;

    bool canAccept(const ValueTypeDesc &valueType);
    std::string toString() const;

    bool operator==(const ValueTypeDesc& other) const {
        return mType == other.mType && mSecondary == other.mSecondary;
    }

    bool operator!=(const ValueTypeDesc &other) const
    {
        return mType != other.mType || mSecondary != other.mSecondary;
    }
};

struct MODULES_EXPORT ExtendedValueTypeDesc {
    ExtendedValueTypeIndex mType;
    ValueTypeSecondaryTypeInfo mSecondary;

    constexpr bool canAccept(const ValueTypeDesc &valueType)
    {
        if (mType == ExtendedValueTypeEnum::GenericType)
            return true;
        return static_cast<ValueTypeDesc>(*this).canAccept(valueType);
    }

    constexpr operator ValueTypeDesc() const
    {
        assert(mType.isRegular());
        return { mType.mType, mSecondary };
    }

    std::string toString() const;
};

template <typename T>
constexpr ValueTypeIndex toValueTypeIndex()
{
    return static_cast<ValueTypeEnum>(type_pack_index_v<size_t, ValueTypeList, T>);
}

template <typename T>
constexpr ExtendedValueTypeDesc toValueTypeDesc()
{
    if constexpr (std::is_same_v<T, ValueType>) {
        return { ExtendedValueTypeEnum::GenericType };
    } else if constexpr (std::is_pointer_v<T>) {
        return { { ValueTypeEnum::ScopeValue }, table_constexpr<std::remove_pointer_t<T>> };
    } else if constexpr (std::is_same_v<T, TypedScopePtr>) {
        return { { ValueTypeEnum::ScopeValue }, static_cast<const MetaTable **>(nullptr) };
    } else {
        return { toValueTypeIndex<T>() };
    }
}

}