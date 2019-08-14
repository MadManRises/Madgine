#pragma once

namespace Engine {

struct AnyHolderBase {
    virtual ~AnyHolderBase() = default;
};

template <typename T>
struct AnyHolder : AnyHolderBase {
	template <typename... Args>
    AnyHolder(Args &&... args)
        : data(std::forward<Args>(args)...)
    {
    }

    T data;
};

struct Any {
    template <typename T>
    struct inplace_t {
    };

    template <typename T>
    static constexpr inline inplace_t<T> inplace = {};

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>, typename... Args>
    Any(inplace_t<T>, Args &&... args)
        : mData(std::make_unique<AnyHolder<std::remove_const_t<std::remove_reference_t<T>>>>(std::forward<Args>(args)...))
    {
    }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>>
    Any(T &&data)
        : mData(std::make_unique < AnyHolder<std::remove_const_t<std::remove_reference_t<T>>>>(std::forward<T>(data)))
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

    template <typename T>
    T &as() const
    {
        AnyHolder<T> *holder = dynamic_cast<AnyHolder<T> *>(mData.get());
        if (!holder)
            throw 0;
        return holder->data;
    }

private:
    std::unique_ptr<AnyHolderBase> mData;
};

}