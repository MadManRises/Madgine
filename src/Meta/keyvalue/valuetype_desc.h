#pragma once

#include "valuetype_types.h"

#include "metatable.h"

#include "Generic/bits.h"

#include "valuetype_forward.h"

#include "Generic/keyvalue.h"

namespace Engine {

enum class ValueTypeEnum : unsigned char {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, ...) Name##Value
#include "valuetypedefinclude.h"
    ,
    MAX_VALUETYPE_TYPE
};

struct META_EXPORT ValueTypeIndex {
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
    GenericType = static_cast<unsigned char>(ValueTypeEnum::MAX_VALUETYPE_TYPE),
    MAX_EXTENDEDVALUETYPE_TYPE
};

struct META_EXPORT ExtendedValueTypeIndex {
    static constexpr size_t bitCount = bitSize(static_cast<unsigned char>(ExtendedValueTypeEnum::MAX_EXTENDEDVALUETYPE_TYPE) - 1);

    BitArray<(sizeof(uintptr_t) * 8) / bitCount, bitCount, ExtendedValueTypeEnum> mTypeList;

    constexpr ExtendedValueTypeIndex(ValueTypeIndex t)
    {
        mTypeList[0] = static_cast<ExtendedValueTypeEnum>(t.mIndex);
    }

    constexpr ExtendedValueTypeIndex(ExtendedValueTypeEnum t)
    {
        mTypeList[0] = t;
    }

    constexpr ExtendedValueTypeIndex(ExtendedValueTypeIndex t, const ExtendedValueTypeIndex &innerKey, const ExtendedValueTypeIndex &innerValue)
    {
        mTypeList[0] = t;
        mTypeList[1] = innerKey.mTypeList[0];
        size_t i = 0;
        int mark = 1;
        do {
            --mark;
            mTypeList[i + 2] = innerValue.mTypeList[i];
            ++i;
            if (mTypeList[i + 2] == static_cast<ExtendedValueTypeEnum>(ValueTypeEnum::KeyValueVirtualRangeValue)) {
                assert(mark == 0);
                mark = 2;
            } 
        } while (mark > 0);
    }

    constexpr bool canAccept(ValueTypeIndex valueType)
    {
        return mTypeList[0] == ExtendedValueTypeEnum::GenericType || static_cast<ValueTypeEnum>(static_cast<ExtendedValueTypeEnum>(mTypeList[0])) == valueType;
    }

    constexpr bool isRegular(size_t level = 0) const
    {
        return static_cast<ValueTypeEnum>(static_cast<ExtendedValueTypeEnum>(mTypeList[level])) < ValueTypeEnum::MAX_VALUETYPE_TYPE;
    }

    constexpr operator ExtendedValueTypeEnum() const
    {
        return mTypeList[0];
    }

    constexpr operator ValueTypeIndex() const
    {
        assert(isRegular());
        return static_cast<ValueTypeEnum>(static_cast<ExtendedValueTypeEnum>(mTypeList[0]));
    }

    constexpr bool operator==(const ExtendedValueTypeIndex &other) const
    {
        return mTypeList == other.mTypeList;
    }

    constexpr bool operator!=(const ExtendedValueTypeIndex &other) const
    {
        return mTypeList != other.mTypeList;
    }

    constexpr bool operator==(const ValueTypeIndex &other) const
    {
        return static_cast<ValueTypeEnum>(static_cast<ExtendedValueTypeEnum>(mTypeList[0])) == other;
    }

    constexpr bool operator!=(const ValueTypeIndex &other) const
    {
        return static_cast<ValueTypeEnum>(static_cast<ExtendedValueTypeEnum>(mTypeList[0])) != other;
    }

    constexpr bool operator==(const ExtendedValueTypeEnum &other) const
    {
        return mTypeList[0] == other;
    }

    constexpr bool operator!=(const ExtendedValueTypeEnum &other) const
    {
        return mTypeList[0] != other;
    }

    std::string_view toString(size_t level = 0) const;
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

struct META_EXPORT ValueTypeDesc {
    ValueTypeIndex mType;
    ValueTypeSecondaryTypeInfo mSecondary;

    bool canAccept(const ValueTypeDesc &valueType);
    std::string toString() const;

    bool operator==(const ValueTypeDesc &other) const
    {
        return mType == other.mType && mSecondary == other.mSecondary;
    }

    bool operator!=(const ValueTypeDesc &other) const
    {
        return mType != other.mType || mSecondary != other.mSecondary;
    }
};

struct META_EXPORT ExtendedValueTypeDesc {
    ExtendedValueTypeIndex mType;
    ValueTypeSecondaryTypeInfo mSecondary;

    constexpr ExtendedValueTypeDesc(ExtendedValueTypeIndex type, ValueTypeSecondaryTypeInfo secondary = {})
        : mType(type)
        , mSecondary(secondary)
    {
    }

    constexpr ExtendedValueTypeDesc(ExtendedValueTypeIndex type, const ExtendedValueTypeDesc &innerKeyDesc, const ExtendedValueTypeDesc &innerValueDesc)
        : mType(type, innerKeyDesc.mType, innerValueDesc.mType)
        , mSecondary(innerValueDesc.mSecondary)
    {
        assert(!innerKeyDesc.mSecondary.mDummy);
    }

    constexpr ExtendedValueTypeDesc(const ValueTypeDesc &desc)
        : mType(desc.mType)
        , mSecondary(desc.mSecondary)
    {
    }

    constexpr bool canAccept(const ValueTypeDesc &valueType)
    {
        if (mType == ExtendedValueTypeEnum::GenericType)
            return true;
        return static_cast<ValueTypeDesc>(*this).canAccept(valueType);
    }

    constexpr operator ValueTypeDesc() const
    {
        assert(mType.isRegular());
        return { mType, mSecondary };
    }

    std::string toString(size_t level = 0) const;
};

template <typename T>
constexpr ValueTypeIndex toValueTypeIndex()
{
    if constexpr (isValueTypePrimitive_v<T>) {
        return static_cast<ValueTypeEnum>(type_pack_index_v<size_t, ValueTypeList, T>);
    } else if constexpr (std::is_pointer_v<T>) {
        return ValueTypeEnum::ScopeValue;
    } else {
        return ValueTypeEnum::OwnedScopeValue;
    }
}

template <typename T>
constexpr ExtendedValueTypeDesc toValueTypeDesc()
{
    if constexpr (isValueTypePrimitive_v<T>) {
        return { toValueTypeIndex<T>() };
    } else if constexpr (std::is_same_v<T, ValueType>) {
        return { ExtendedValueTypeEnum::GenericType };
    } else if constexpr (std::is_same_v<T, TypedScopePtr>) {
        return { { ValueTypeEnum::ScopeValue }, static_cast<const MetaTable **>(nullptr) };
    } else if constexpr (is_iterable_v<T>) {
        return { { ValueTypeEnum::KeyValueVirtualRangeValue }, toValueTypeDesc<KeyType_t<typename T::iterator::value_type>>(), toValueTypeDesc<ValueType_t<typename T::iterator::value_type>>() };
    } else if constexpr (std::is_pointer_v<T>) {
        return { { ValueTypeEnum::ScopeValue }, &table<std::remove_pointer_t<T>> };
    } else {
        return { { ValueTypeEnum::OwnedScopeValue }, &table<T> };
    } 
}

}