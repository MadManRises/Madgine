#pragma once

#include "../datamutex.h"
#include "Generic/execution/concepts.h"

namespace Engine {
namespace Threading {

    struct with_lock_t {

        template <typename Rec, typename F>
        struct state : Execution::base_state<Rec> {

            void start()
            {
                DataMutex::Lock lock = mMutex.tryLock(mQueue);
                if (lock.isHeld()) {
                    TupleUnpacker::invoke(mF, std::move(lock));
                    lock = {};
                    set_value();
                } else {
                    mQueue->queue([this]() -> ImmediateTask<void> {
                        {
                            DataMutex::Lock lock = co_await mMutex;
                            TupleUnpacker::invoke(mF, std::move(lock));
                        }
                        set_value();
                    });
                }
            }

            DataMutex::Moded mMutex;
            TaskQueue *mQueue;
            F mF;
        };

        template <typename F>
        struct sender : Execution::base_sender {

            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;
            using result_type = void;

            template <typename Rec>
            friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, F> { std::forward<Rec>(rec), sender.mMutex, sender.mQueue, std::forward<F>(sender.mF) };
            }

            DataMutex::Moded mMutex;
            TaskQueue *mQueue;
            F mF;
        };

        template <typename F>
        friend auto tag_invoke(with_lock_t, DataMutex::Moded mutex, TaskQueue *queue, F &&f)
        {
            return sender<F> { {}, mutex, queue, std::forward<F>(f) };
        }

        template <typename F>
        auto operator()(DataMutex::Moded mutex, TaskQueue *queue, F &&f) const
            noexcept(is_nothrow_tag_invocable_v<with_lock_t, DataMutex::Moded, TaskQueue *, F>)
                -> tag_invoke_result_t<with_lock_t, DataMutex::Moded, TaskQueue *, F>
        {
            return tag_invoke(*this, mutex, queue, std::forward<F>(f));
        }
    };

    inline constexpr with_lock_t with_lock;

    struct start_with_lock_t {

        template <typename Rec>
        struct receiver : Execution::algorithm_receiver<Rec> {
            using base = Execution::algorithm_receiver<Rec>;

            template <typename... V>
            void set_value(V &&...value)
            {
                mLock = {};
                base::set_value(std::forward<V>(value)...);
            }

            void set_done()
            {
                mLock = {};
                base::set_done();
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mLock = {};
                base::set_error(std::forward<R>(result)...);
            }

            DataMutex::Lock &mLock;
        };

        template <typename Rec, typename Sender>
        struct state : Execution::algorithm_state<Sender, receiver<Rec>> {
            using base = Execution::algorithm_state<Sender, receiver<Rec>>;

            state(Rec &&rec, DataMutex::Moded mutex, TaskQueue *queue, Sender &&inner)
                : base(std::forward<Sender>(inner), std::forward<Rec>(rec), mLock)
                , mMutex(std::move(mutex))
                , mQueue(queue)
            {
            }

            void start()
            {
                mLock = mMutex.tryLock(mQueue);
                if (mLock.isHeld()) {
                    base::start();
                    mLock = {};
                } else {
                    mQueue->queue([this]() -> ImmediateTask<void> {
                        {
                            mLock = co_await mMutex;
                            base::start();
                            mLock = {};
                        }
                    });
                }
            }

            DataMutex::Moded mMutex;
            TaskQueue *mQueue;
            DataMutex::Lock mLock;
        };

        template <typename Sender>
        struct sender : Execution::algorithm_sender<Sender> {

            template <typename Rec>
            friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender> { std::forward<Rec>(rec), sender.mMutex, sender.mQueue, std::forward<Sender>(sender.mSender) };
            }

            DataMutex::Moded mMutex;
            TaskQueue *mQueue;
            Sender mSender;
        };

        template <typename Sender>
        friend auto tag_invoke(start_with_lock_t, DataMutex::Moded mutex, TaskQueue *queue, Sender &&inner)
        {
            return sender<Sender> { {}, mutex, queue, std::forward<Sender>(inner) };
        }

        template <typename Sender>
        auto operator()(DataMutex::Moded mutex, TaskQueue *queue, Sender &&sender) const
            noexcept(is_nothrow_tag_invocable_v<start_with_lock_t, DataMutex::Moded, TaskQueue *, Sender>)
                -> tag_invoke_result_t<start_with_lock_t, DataMutex::Moded, TaskQueue *, Sender>
        {
            return tag_invoke(*this, mutex, queue, std::forward<Sender>(sender));
        }
    };

    inline constexpr start_with_lock_t start_with_lock;

}
}