#pragma once

namespace Engine {
namespace Execution {

    struct detach_t {
        template <typename S, typename R, typename... V>
        struct receiver {
            void set_value(V...)
            {
                delete mState;
            }
            void set_done()
            {
                delete mState;
            }
            void set_error(patch_void_t<R>)
            {
                delete mState;
            }
            S *mState;
        };

        template <typename F, typename R, typename... V>
        struct state {
            state(Sender<F, R, V...> &&sender)
                : mState(sender(receiver<state, R, V...> { this }))
            {
            }
            void start()
            {
                mState.start();
            }

            std::invoke_result_t<F, receiver<state, R, V...>> mState;
        };

        template <typename F, typename R, typename... V>
        void operator()(Sender<F, R, V...> &&sender) const
        {
            (new state<F, R, V...> { std::move(sender) })->start();
        }
    };

    inline constexpr detach_t detach;

}
}
