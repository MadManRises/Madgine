#pragma once

#include "Generic/tag_invoke.h"

namespace Engine {

struct destruct_t {
    template <typename T, typename... Args>
    requires tag_invocable<destruct_t, T, Args...>
    auto operator()(T &&object) const
        noexcept(is_nothrow_tag_invocable_v<destruct_t, T>)
            -> tag_invoke_result_t<destruct_t, T>
    {
        return tag_invoke(*this, std::forward<T>(object));
    }
};

inline constexpr destruct_t destruct;

struct construct_t {
    template <typename T, typename... Args>
    requires tag_invocable<construct_t, T, Args...>
    auto operator()(T &&object, Args &&...args) const
        noexcept(is_nothrow_tag_invocable_v<construct_t, T, Args...>)
            -> tag_invoke_result_t<construct_t, T, Args...>
    {
        return tag_invoke(*this, std::forward<T>(object), std::forward<Args>(args)...);
    }
};

inline constexpr construct_t construct;

template <typename T>
struct ManualLifetime {

    ManualLifetime(std::nullopt_t)
        : mAlive(false)
    {
    }

    template <DecayedNoneOf<ManualLifetime<T>>... Args>
    ManualLifetime(Args &&...args)
        : mData(std::forward<Args>(args)...)
        , mAlive(true)
    {
    }

    ManualLifetime(const ManualLifetime &)
    {
        throw 0;
    }

    ManualLifetime(ManualLifetime &&)
    {
        throw 0;
    }

    ~ManualLifetime() { assert(!mAlive); };

    operator T &() &
    {
        assert(mAlive);
        return mData;
    }

    operator const T &() const
    {
        assert(mAlive);
        return mData;
    }

    operator T &&() &&
    {
        assert(mAlive);
        return mData;
    }

    template <typename U>
    explicit operator U() const requires std::convertible_to<T, U>
    {
        return static_cast<U>(mData);
    }

    T *operator&()
    {
        assert(mAlive);
        return &mData;
    }

    const T *operator&() const
    {
        assert(mAlive);
        return &mData;
    }

    T *operator->()
    {
        assert(mAlive);
        return &mData;
    }

    const T *operator->() const
    {
        assert(mAlive);
        return &mData;
    }

    T &unsafeAccess()
    {
        return mData;
    }

    const T &unsafeAccess() const
    {
        return mData;
    }

    ManualLifetime &operator=(T &&t)
    {
        assert(mAlive && t.mAlive);
        mData = std::forward<T>(t);
        return *this;
    }

    ManualLifetime &operator=(ManualLifetime &&t)
    {
        assert(mAlive && t.mAlive);
        mData = std::move(t.mData);
        return *this;
    }

    template <typename... Args>
    friend auto tag_invoke(construct_t, ManualLifetime &object, Args &&...args)
    {
        assert(!object.mAlive);
        object.mAlive = true;
        new (&object.mData) T(std::forward<Args>(args)...);
    }

    friend auto tag_invoke(destruct_t, ManualLifetime &object)
    {
        assert(object.mAlive);
        object.mData.~T();
        object.mAlive = false;
    }

    /* template <typename CPO, typename... Args>
    friend auto tag_invoke(CPO cpo, ManualLifetime& object, Args &&...args) 
        -> tag_invoke_result_t<CPO, T&, Args...>
    {
        return tag_invoke(cpo, static_cast<T &>(object), std::forward<Args>(args)...);
    }*/

    union {
        T mData;
    };
    bool mAlive = false;
};

}