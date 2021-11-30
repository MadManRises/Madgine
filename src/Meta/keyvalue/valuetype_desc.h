#pragma once

#include "valuetype_types.h"

#include "functiontable.h"
#include "metatable.h"

#include "Generic/bits.h"

#include "Generic/keyvalue.h"

namespace Engine {

template <typename T>
using isValueTypePrimitive = type_pack_contains<ValueTypeList, T>;

template <typename T>
static constexpr bool isValueTypePrimitive_v = isValueTypePrimitive<T>::value;

template <typename T>
concept ValueTypePrimitive = isValueTypePrimitive_v<T>;

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
    OptionalType,
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

    constexpr ExtendedValueTypeIndex(ExtendedValueTypeIndex t, const ExtendedValueTypeIndex &inner)
    {
        mTypeList[0] = t;
        size_t i = 0;
        int mark = 1;
        do {
            --mark;
            mTypeList[i + 1] = inner.mTypeList[i];
            ++i;
            if (mTypeList[i + 1] == static_cast<ExtendedValueTypeEnum>(ValueTypeEnum::KeyValueVirtualAssociativeRangeValue)) {
                assert(mark == 0);
                mark = 2;
            }
            if (mTypeList[i + 1] == static_cast<ExtendedValueTypeEnum>(ValueTypeEnum::KeyValueVirtualSequenceRangeValue)) {
                assert(mark == 0);
                mark = 1;
            }
        } while (mark > 0);
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
            if (mTypeList[i + 2] == static_cast<ExtendedValueTypeEnum>(ValueTypeEnum::KeyValueVirtualAssociativeRangeValue)) {
                assert(mark == 0);
                mark = 2;
            }
            if (mTypeList[i + 2] == static_cast<ExtendedValueTypeEnum>(ValueTypeEnum::KeyValueVirtualSequenceRangeValue)) {
                assert(mark == 0);
                mark = 1;
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

    constexpr std::strong_ordering operator<=>(const ExtendedValueTypeIndex &other) const = default;
    constexpr bool operator==(const ExtendedValueTypeIndex &other) const = default;

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

    std::strong_ordering compare(ValueTypeIndex index, const ValueTypeSecondaryTypeInfo &other) const
    {
        switch (index) {
        case ValueTypeEnum::ScopeValue:
            return *mMetaTable <=> *other.mMetaTable;
        case ValueTypeEnum::ApiFunctionValue:
        case ValueTypeEnum::BoundApiFunctionValue:
            return *mFunctionTable <=> *other.mFunctionTable;
        default:
            return mDummy <=> other.mDummy;
        }
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

    std::strong_ordering operator<=>(const ValueTypeDesc &other) const
    {
        if (auto cmp = mType <=> other.mType; cmp != 0)
            return cmp;
        return mSecondary.compare(mType, other.mSecondary);
    }
    bool operator==(const ValueTypeDesc &other) const
    {
        return mType == other.mType && mSecondary.compare(mType, other.mSecondary) == 0;
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

    constexpr ExtendedValueTypeDesc(ExtendedValueTypeIndex type, const ExtendedValueTypeDesc &innerDesc)
        : mType(type, innerDesc.mType)
        , mSecondary(innerDesc.mSecondary)
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

    constexpr bool canAccept(const ValueTypeDesc &valueType) const
    {
        if (mType == ExtendedValueTypeEnum::GenericType)
            return true;
        return static_cast<ValueTypeDesc>(*this).canAccept(valueType);
    }

    constexpr bool isCompatible(const ExtendedValueTypeDesc &valueType)
    {
        if (mType.isRegular())
            return valueType.canAccept(*this);
        if (valueType.mType.isRegular())
            return canAccept(valueType);
        return mType == valueType.mType;
    }

    constexpr operator ValueTypeDesc() const
    {
        assert(mType.isRegular());
        return { mType, mSecondary };
    }

    constexpr std::strong_ordering operator<=>(const ExtendedValueTypeDesc &other) const
    {
        if (auto cmp = mType <=> other.mType; cmp != 0)
            return cmp;
        return mSecondary.compare(mType, other.mSecondary);
    }
    constexpr bool operator==(const ExtendedValueTypeDesc& other) const {
        return mType == other.mType && mSecondary.compare(mType, other.mSecondary) == 0;
    }

    std::string toString(size_t level = 0) const;
};

template <typename T>
constexpr ValueTypeIndex toValueTypeIndex()
{
    static_assert(!std::is_same_v<T, ValueType>);
    if constexpr (isValueTypePrimitive_v<T>) {
        return static_cast<ValueTypeEnum>(type_pack_index_v<size_t, ValueTypeList, T>);
    } else if constexpr (Iterable<T>) {
        if constexpr (std::is_same_v<KeyType_t<typename T::iterator::value_type>, std::monostate>)
            return ValueTypeEnum::KeyValueVirtualSequenceRangeValue;
        else
            return ValueTypeEnum::KeyValueVirtualAssociativeRangeValue;
    } else if constexpr (Pointer<T>) {
        if constexpr (std::is_function_v<std::remove_pointer_t<T>>)
            return ValueTypeEnum::FunctionValue;
        else if constexpr (std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, FunctionTable>)
            return ValueTypeEnum::ApiFunctionValue;
        else
            return ValueTypeEnum::ScopeValue;
    } else {
        return ValueTypeEnum::OwnedScopeValue;
    }
}

template <typename T>
constexpr ExtendedValueTypeDesc toValueTypeDesc()
{
    if constexpr (InstanceOf<T, std::optional>) {
        return { { ExtendedValueTypeEnum::OptionalType }, toValueTypeDesc<type_pack_unpack_unique_t<typename is_instance<T, std::optional>::argument_types>>() };
    } else if constexpr (isValueTypePrimitive_v<T>) {
        return { toValueTypeIndex<T>() };
    } else if constexpr (std::same_as<T, ValueType>) {
        return { ExtendedValueTypeEnum::GenericType };
    } else if constexpr (std::same_as<T, TypedScopePtr>) {
        return { { ValueTypeEnum::ScopeValue }, static_cast<const MetaTable **>(nullptr) };
    } else if constexpr (Iterable<T>) {
        if constexpr (std::same_as<KeyType_t<typename T::iterator::value_type>, std::monostate>)
            return { { ValueTypeEnum::KeyValueVirtualSequenceRangeValue }, toValueTypeDesc<typename T::iterator::value_type>() };
        else
            return { { ValueTypeEnum::KeyValueVirtualAssociativeRangeValue }, toValueTypeDesc<KeyType_t<typename T::iterator::value_type>>(), toValueTypeDesc<ValueType_t<typename T::iterator::value_type>>() };
    } else if constexpr (Pointer<T>) {
        if constexpr (Function<std::remove_pointer_t<T>> || std::same_as<std::remove_cv_t<std::remove_pointer_t<T>>, FunctionTable>)
            //return { { ValueTypeEnum::ApiFunctionValue }, nullptr };
            throw 0;
        else
            return { { ValueTypeEnum::ScopeValue }, &table<std::remove_pointer_t<T>> };
    } else {
        return { { ValueTypeEnum::OwnedScopeValue }, &table<T> };
    }
}
}