#pragma once

#include "any.h"

#include "tupleunpacker.h"

namespace Engine {

struct ByteBufferDataAccessor {
    template <typename T>
    const void *operator()(const std::vector<T> &v) { return v.data(); }

    template <typename T>
    const void *operator()(const std::unique_ptr<T> &p) { return p.get(); }
};
struct ByteBufferSizeAccessor {
    template <typename T>
    size_t operator()(const std::vector<T> &v) { return v.size() * sizeof(T); }
};

struct ByteBuffer {

    ByteBuffer() = default;

    template <typename T, typename SizeAccessor = ByteBufferSizeAccessor, typename DataAccessor = ByteBufferDataAccessor,
        typename = std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>> && !std::is_null_pointer_v<std::remove_reference_t<T>> && !std::is_convertible_v<SizeAccessor, size_t> && !std::is_convertible_v<DataAccessor, const void *>>>
    ByteBuffer(T &&t, SizeAccessor &&sizeAccess = {}, DataAccessor &&dataAccess = {})
        : mKeep(std::forward<T>(t))
        , mSize(TupleUnpacker::invoke(std::forward<SizeAccessor>(sizeAccess), mKeep.as<T>()))
        , mData(TupleUnpacker::invoke(std::forward<DataAccessor>(dataAccess), mKeep.as<T>()))
    {
    }

    template <typename T, typename DataAccessor = ByteBufferDataAccessor,
        typename = std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>> && !std::is_null_pointer_v<std::remove_reference_t<T>> && !std::is_convertible_v<DataAccessor, const void *>>>
    ByteBuffer(T &&t, size_t size, DataAccessor &&dataAccess = {})
        : mKeep(std::forward<T>(t))
        , mSize(size)
        , mData(TupleUnpacker::invoke(std::forward<DataAccessor>(dataAccess), mKeep.as<T>()))
    {
    }

    template <typename T, typename = std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>> && !std::is_null_pointer_v<std::remove_reference_t<T>>>>
    ByteBuffer(T &&t, size_t size, const void *data)
        : mKeep(std::forward<T>(t))
        , mSize(size)
        , mData(data)
    {
    }

    ByteBuffer(const void *data, size_t size)
        : mSize(size)
        , mData(data)
    {
    }

private:
    Any mKeep;

public:
    size_t mSize = 0;
    const void *mData = nullptr;
};

}