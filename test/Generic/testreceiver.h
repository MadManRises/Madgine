#pragma once

template <typename T, typename R>
struct TestReceiver {

    void set_value(R result, T value)
    {
        mValue = std::forward<T>(value);
        mResult = std::forward<R>(result);
        mFinished = true;
    }

    void set_done()
    {
        mCanceled = true;
    }

    void set_error(R result)
    {
        mResult = std::forward<R>(result);
        mFinished = true;
    }

    bool is_ready()
    {
        return mFinished;
    }

    std::optional<T> mValue;
    R mResult;

    bool mFinished = false;
    bool mCanceled = false;
};

template <typename R>
struct TestReceiver<void, R> {

    void set_value(R result)
    {
        mHasValue = true;
        mResult = std::forward<R>(result);
        mFinished = true;
    }

    void set_done()
    {
        mCanceled = true;
    }

    void set_error(R result)
    {
        mResult = std::forward<R>(result);
        mFinished = true;
    }

    bool is_ready()
    {
        return mFinished;
    }

    bool mHasValue;
    R mResult;

    bool mFinished = false;
    bool mCanceled = false;
};

struct GenericTestReceiver {
    template <typename R, typename T>
    void set_value(R result, T value)
    {
        mFinished = true;
    }

    void set_done()
    {
        mCanceled = true;
    }

    template <typename R>
    void set_error(R result)
    {        
        mFinished = true;
    }

    bool is_ready()
    {
        return mFinished;
    }

    void reset() {
        mFinished = false;
        mCanceled = false;
    }

    bool mFinished = false;
    bool mCanceled = false;
};