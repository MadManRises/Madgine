#pragma once

#include "inheritable.h"

namespace Engine {

struct AnyHolderBase {
    virtual ~AnyHolderBase() = default;
};

template <typename T>
struct AnyHolder : AnyHolderBase, Inheritable<T> {
    template <typename... Args>
    AnyHolder(Args &&...args)
        : Inheritable<T>(std::forward<Args>(args)...)
    {
    }
};

struct Any {
    template <typename T>
    struct inplace_t {
    };

    template <typename T>
    static constexpr inline inplace_t<T> inplace = {};

    Any() = default;

    template <typename T, typename... Args>
    requires(!std::same_as<std::decay_t<T>, Any>)
    Any(inplace_t<T>, Args &&... args)
        : mData(std::make_unique<AnyHolder<T>>(std::forward<Args>(args)...))
    {
    }

    template <typename T>
    requires(!std::same_as<std::decay_t<T>, Any>)
    Any(T &&data)
        : mData(std::make_unique<AnyHolder<T>>(std::forward<T>(data)))
    {
    }

    Any(Any &&other)
        : mData(std::move(other.mData))
    {
    }

    Any &operator=(Any &&other)
    {
        mData = std::move(other.mData);
        return *this;
    }

    explicit operator bool() const
    {
        return mData.operator bool();
    }

    template <typename T>
    T &as() const
    {
        AnyHolder<T> *holder = dynamic_cast<AnyHolder<T> *>(mData.get());
        if (!holder)
            std::terminate();
        return *holder;
    }

private:
    std::unique_ptr<AnyHolderBase> mData;
};

}