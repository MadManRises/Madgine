#pragma once

#include "any.h"

namespace Engine {

struct ByteBufferDataAccessor {
    template <typename T>
    auto operator()(const std::vector<T> &v) { return v.data(); }

    template <typename T>
    auto operator()(std::vector<T> &v) { return v.data(); }

    template <typename T, typename D>
    auto operator()(const std::unique_ptr<T, D> &p) { return p.get(); }
};
struct ByteBufferSizeAccessor {
    template <typename T>
    size_t operator()(const std::vector<T> &v) { return v.size() * sizeof(T); }
};

template <typename Data>
struct ByteBufferImpl {

    ByteBufferImpl() = default;

    template <typename T, typename SizeAccessor = ByteBufferSizeAccessor, typename DataAccessor = ByteBufferDataAccessor>
    requires(!Pointer<std::remove_reference_t<T>> && !std::convertible_to<SizeAccessor, size_t> && !std::convertible_to<DataAccessor, const void *>)
        ByteBufferImpl(T &&t, SizeAccessor &&sizeAccess = {}, DataAccessor &&dataAccess = {})
        : mKeep(std::forward<T>(t))
        , mSize(TupleUnpacker::invoke(std::forward<SizeAccessor>(sizeAccess), mKeep.as<T>()))
        , mData(TupleUnpacker::invoke(std::forward<DataAccessor>(dataAccess), mKeep.as<T>()))
    {
    }

    template <typename T, typename DataAccessor = ByteBufferDataAccessor>
    requires(!Pointer<std::remove_reference_t<T>> && !std::convertible_to<DataAccessor, const void *>)
        ByteBufferImpl(T &&t, size_t size, DataAccessor &&dataAccess = {})
        : mKeep(std::forward<T>(t))
        , mSize(size)
        , mData(TupleUnpacker::invoke(std::forward<DataAccessor>(dataAccess), mKeep.as<T>()))
    {
    }

    template <typename T>
    requires(!std::is_pointer_v<std::remove_reference_t<T>> && !std::is_null_pointer_v<std::remove_reference_t<T>>)
        ByteBufferImpl(T &&t, size_t size, Data *data)
        : mKeep(std::forward<T>(t))
        , mSize(size)
        , mData(data)
    {
    }

    ByteBufferImpl(const void *data, size_t size)
        : mSize(size)
        , mData(data)
    {
    }

    void clear()
    {
        mSize = 0;
        mData = nullptr;
        mKeep = {};
    }

    template <typename T>
    ByteBufferImpl<T> cast() &&
    {
        return {
            std::move(mKeep),
            mSize,
            static_cast<std::remove_extent_t<T> *>(mData)
        };
    }

    Data *begin() const
    {
        return mData;
    }

    Data *end() const
    {
        return mData + mSize;
    }

    Data *operator->() const
    {
        return mData;
    }

    bool operator==(const ByteBufferImpl &other) const
    {
        if (mSize != other.mSize)
            return false;
        return std::memcmp(mData, other.mData, mSize) == 0;
    }

private:
    Any mKeep;

public:
    size_t mSize = 0;
    Data *mData = nullptr;
};

template <typename Data>
struct ByteBufferImpl<Data[]> : ByteBufferImpl<Data> {

    using ByteBufferImpl<Data>::ByteBufferImpl;

    Data &operator[](size_t index)
    {
        return this->mData[index];
    }
};

using ByteBuffer = ByteBufferImpl<const void>;
using WritableByteBuffer = ByteBufferImpl<void>;

}