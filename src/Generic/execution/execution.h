#pragma once

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

}
}
