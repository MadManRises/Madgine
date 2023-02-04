#pragma once

namespace Engine {

template <typename T, typename R>
struct WithResult {
    WithResult() = default;
    WithResult(T t)
        : mData(std::move(t))
    {
    }
    WithResult(R result, std::optional<T> data = {})
        : mData(std::move(data))
        , mResult(result)
    {
    }

    R get(T &data)
    {
        if (mData)
            data = std::move(*mData);
        return mResult;
    }

    operator T()
    {
        return *mData;
    }

    std::optional<T> mData;
    R mResult;
};

template <typename R>
struct WithResult<void, R> {
    WithResult() = default;
    WithResult(R result)
        : mResult(result)
    {
    }

    R get()
    {
        return mResult;
    }

    R mResult;
};

}