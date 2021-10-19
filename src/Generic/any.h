#pragma once

namespace Engine {

struct AnyHolderBase {
    virtual ~AnyHolderBase() = default;
};

template <typename T, bool isClass>
struct AnyHolder : AnyHolderBase, T {
    template <typename... Args>
    AnyHolder(Args &&... args)
        : T(std::forward<Args>(args)...)
    {
    }    
};

template <typename T>
struct AnyHolder<T, false> : AnyHolderBase {
    template <typename... Args>
    AnyHolder(Args &&... args)
        : data(std::forward<Args>(args)...)
    {
    }

    operator T& () {
        return data;
    }

    T data;
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
        : mData(std::make_unique<AnyHolder<T, std::is_class_v<T>>>(std::forward<Args>(args)...))
    {
    }

    template <typename T>
    requires(!std::same_as<std::decay_t<T>, Any>)
    Any(T &&data)
        : mData(std::make_unique<AnyHolder<T, std::is_class_v<T>>>(std::forward<T>(data)))
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
        AnyHolder<T, std::is_class_v<T>> *holder = dynamic_cast<AnyHolder<T, std::is_class_v<T>> *>(mData.get());
        if (!holder)
            std::terminate();
        return *holder;
    }

private:
    std::unique_ptr<AnyHolderBase> mData;
};

}