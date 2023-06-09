#pragma once

#include "../makeowning.h"
#include "../withresult.h"
#include "concepts.h"

namespace Engine {
namespace Execution {

    struct detach_t {

        template <typename Sender>
        struct state;

        template <typename Sender>
        struct receiver : execution_receiver<> {

            template <typename... V>
            void set_value(V &&...)
            {
                delete mState;
            }
            void set_done()
            {
                delete mState;
            }
            template <typename... R>
            void set_error(R &&...)
            {
                delete mState;
            }

            state<Sender> *mState;
        };

        template <typename Sender>
        struct state {
            state(Sender &&sender)
                : mState(connect(std::forward<Sender>(sender), receiver<Sender> { {}, this }))
            {
            }
            void start()
            {
                mState.start();
            }

            connect_result_t<Sender, receiver<Sender>> mState;
        };

        template <typename Sender>
        void operator()(Sender &&sender) const
        {
            (new state<Sender> { std::forward<Sender>(sender) })->start();
        }
    };

    inline constexpr detach_t detach;

    struct sync_expect_t {

        template <typename Sender>
        struct state;

        template <typename Sender>
        struct receiver : execution_receiver<> {

            template <typename... V>
            void set_value(V &&... v)
            {
                mState->mResult = { std::forward<V>(v)... };
                mState->mFinished = true;
            }
            void set_done()
            {
                mState->mFinished = true;
            }
            template <typename R>
            void set_error(R &&r)
            {
                mState->mResult = std::forward<R>(r);
                mState->mFinished = true;
            }

            state<Sender> *mState;
        };

        template <typename Sender>
        struct state {
            state(Sender &&sender)
                : mState(connect(std::forward<Sender>(sender), receiver<Sender> { {}, this }))
            {
            }
            void start()
            {
                mState.start();

            }

            template <typename... T>
            using helper = typename type_pack<T...>::template unpack_unique<void>;
            WithResult<typename Sender::result_type, MakeOwning_t<typename Sender::template value_types<helper>>> mResult;
            bool mFinished = false;
            connect_result_t<Sender, receiver<Sender>> mState;
        };

        template <typename Sender>
        auto operator()(Sender &&sender) const
        {
            state<Sender> state { std::forward<Sender>(sender) };
            state.start();
            return state.mResult;
        }
    };

    inline constexpr sync_expect_t sync_expect;

}
}
