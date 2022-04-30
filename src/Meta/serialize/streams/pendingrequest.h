#pragma once

#include "Generic/future.h"
#include "Generic/lambda.h"
#include "messageresult.h"

namespace Engine {
namespace Serialize {

    template <typename T>
    struct MessageData {
        MessageData() = default;
        MessageData(T t)
            : mData(std::move(t))
        {
        }
        MessageData(MessageResult result, std::optional<T> data = {})
            : mResult(result)
            , mData(std::move(data))
        {
        }

        MessageResult get(T &result)
        {
            if (mData)
                result = *mData;
            return mResult;
        }

        operator T()
        {
            return *mData;
        }

        MessageResult mResult = MessageResult::OK;
        std::optional<T> mData;
    };

    template <>
    struct MessageData<void> {
        MessageData() = default;
        MessageData(MessageResult result)
            : mResult(result)
        {
        }

        MessageResult get()
        {
            return mResult;
        }

        MessageResult mResult = MessageResult::OK;
    };

    template <typename T>
    using MessagePromise = std::promise<MessageData<T>>;

    struct GenericMessagePromise {

        GenericMessagePromise() = default;

        template <typename T>
        GenericMessagePromise(MessagePromise<T> promise)
            : mPromiseCallback([promise { std::move(promise) }](MessageResult result, const void *data) mutable {
                if constexpr (std::same_as<T, void>) {
                    promise.set_value({ result });
                } else if (data) {
                    promise.set_value({ result, *static_cast<const T *>(data) });
                } else {
                    promise.set_value({ result, std::nullopt });
                }
            })
        {
        }

        void setResult(MessageResult result)
        {
            if (mPromiseCallback)
                mPromiseCallback(result, nullptr);
        }
        template <typename T>
        void setValue(const T &t)
        {
            if (mPromiseCallback)
                mPromiseCallback(MessageResult::OK, &t);
        }

        explicit operator bool() const
        {
            return static_cast<bool>(mPromiseCallback);
        }

        Lambda<void(MessageResult, const void *)> mPromiseCallback;
    };

    template <typename T>
    struct MessageFuture {

        using type = T;

        MessageFuture() = default;

        MessageFuture(MessageFuture &&) = default;

        MessageFuture(T t)
            : mFuture(std::move(t))
        {
        }

        MessageFuture(Future<MessageData<T>> future)
            : mFuture(std::move(future))
        {
        }

        MessageFuture &operator=(MessageFuture &&) = default;

        MessageResult get(T &result)
        {
            return mFuture.get().get(result);
        }

        operator T()
        {
            return mFuture.get();
        }

        bool is_ready() const
        {
            return mFuture.is_ready();
        }

        template <typename F>
        MessageFuture<std::invoke_result_t<F, T>> then(F &&f)
        {
            return Future<MessageData<std::invoke_result_t<F, T>>> { mFuture.then([f { std::forward<F>(f) }](MessageData<T> &&data) -> MessageData<std::invoke_result_t<F, T>> {
                if (data.mData) {
                    return { data.mResult, f(*data.mData) };
                } else {
                    return { data.mResult };
                }
            }) };
        }

    private:
        Future<MessageData<T>> mFuture;
    };

    template <>
    struct MessageFuture<void> {

        using type = void;

        MessageFuture() = default;

        MessageFuture(MessageFuture &&) = default;

        MessageFuture(Void v)
            : mFuture(MessageData<void> {})
        {
        }

        MessageFuture(Future<MessageData<void>> future)
            : mFuture(std::move(future))
        {
        }

        MessageFuture &operator=(MessageFuture &&) = default;

        MessageResult get()
        {
            return mFuture.get().get();
        }

        bool is_ready() const
        {
            return mFuture.is_ready();
        }

    private:
        Future<MessageData<void>> mFuture;
    };

    struct PendingRequest {

        PendingRequest(MessageId id = 0, ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessagePromise promise = {})
            : mId(id)
            , mRequester(requester)
            , mRequesterTransactionId(requesterTransactionId)
            , mPromise(std::move(promise))
        {
        }

        PendingRequest(PendingRequest &&other)
            : mId(std::exchange(other.mId, 0))
            , mRequester(std::exchange(other.mRequester, 0))
            , mRequesterTransactionId(std::exchange(other.mRequesterTransactionId, 0))
            , mPromise(std::move(other.mPromise))
        {
        }

        ~PendingRequest()
        {
        }

        PendingRequest &operator=(PendingRequest &&other)
        {
            mId = std::exchange(other.mId, 0);
            mRequester = std::exchange(other.mRequester, 0);
            mRequesterTransactionId = std::exchange(other.mRequesterTransactionId, 0);
            mPromise = std::move(other.mPromise);
            return *this;
        }

        MessageId mId = 0;
        ParticipantId mRequester = 0;
        MessageId mRequesterTransactionId = 0;
        GenericMessagePromise mPromise;
    };

}
}