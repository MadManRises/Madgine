#pragma once

#include "Generic/execution/sender.h"
#include "Generic/execution/virtualreceiver.h"
#include "Generic/forward_capture.h"
#include "Generic/nulledptr.h"
#include "Generic/withresultfuture.h"
#include "messageresult.h"

namespace Engine {
namespace Serialize {

    template <typename T>
    using MessageData = WithResult<T, MessageResult>;

    template <typename T>
    using MessagePromise = std::promise<MessageData<T>>;

    template <typename T>
    using MessageFuture = WithResultFuture<T, MessageResult>;

    struct GenericMessageReceiver {

        GenericMessageReceiver() = default;
        GenericMessageReceiver(Execution::VirtualReceiverBase<const void *, MessageResult> &receiver)
            : mPtr(&receiver)
        {
        }
        GenericMessageReceiver(GenericMessageReceiver &&) = default;
        ~GenericMessageReceiver() {
            assert(!mPtr);
        }

        GenericMessageReceiver &operator=(GenericMessageReceiver &&) = default;

        template <typename T>
        void set_value(MessageResult result, const T &t)
        {
            if (mPtr) {
                mPtr->set_value(result, &t);
                mPtr.reset();
            }
        }
        void set_done()
        {
            if (mPtr) {
                mPtr->set_done();
                mPtr.reset();
            }
        }
        void set_error(MessageResult result)
        {
            if (mPtr) {
                mPtr->set_error(result);
                mPtr.reset();
            }
        }

        explicit operator bool() const
        {
            return mPtr;
        }

    private:
        NulledPtr<Execution::VirtualReceiverBase<const void *, MessageResult>> mPtr;
    };

    template <typename Rec, typename T>
    struct MessageReceiver : Execution::VirtualReceiverBase<const void *, MessageResult> {
        MessageReceiver(Rec &&rec)
            : mRec(std::forward<Rec>(rec))
        {
        }
        void set_value(T &&value)
        {
            mRec.set_value(MessageResult::OK, std::forward<T>(value));
        }
        virtual void set_value(MessageResult result, const void *data) override final
        {
            assert(data);
            if constexpr (std::same_as<T, void>) {
                mRec.set_value(result);
            } else if (data) {
                mRec.set_value(result, *static_cast<const T *>(data));
            }
        }
        virtual void set_done() override final
        {
            mRec.set_done();
        }
        virtual void set_error(MessageResult result) override final
        {
            mRec.set_error(result);
        }
        Rec mRec;
    };

    template <typename T, typename F, typename... Args>
    auto make_message_sender(F &&f, Args &&...args)
    {
        return Execution::make_sender<T, MessageResult>(
            [f { forward_capture(std::forward<F>(f)) }, args = std::tuple<Args...> { std::forward<Args>(args)... }]<typename Rec>(Rec &&rec) mutable {
                struct State {
                    void start()
                    {
                        TupleUnpacker::invokeExpand(std::forward<F>(mF), mRec, std::move(mArgs));
                    }
                    F mF;
                    MessageReceiver<Rec, T> mRec;
                    std::tuple<Args...> mArgs;
                };
                return State { std::forward<F>(f), std::forward<Rec>(rec), std::move(args) };
            });
    }

    struct PendingRequest {

        PendingRequest(MessageId id = 0, ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessageReceiver receiver = {})
            : mId(id)
            , mRequester(requester)
            , mRequesterTransactionId(requesterTransactionId)
            , mReceiver(std::move(receiver))
        {
        }

        PendingRequest(PendingRequest &&other)
            : mId(std::exchange(other.mId, 0))
            , mRequester(std::exchange(other.mRequester, 0))
            , mRequesterTransactionId(std::exchange(other.mRequesterTransactionId, 0))
            , mReceiver(std::move(other.mReceiver))
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
            mReceiver = std::move(other.mReceiver);
            return *this;
        }

        MessageId mId = 0;
        ParticipantId mRequester = 0;
        MessageId mRequesterTransactionId = 0;
        GenericMessageReceiver mReceiver;
    };

}
}