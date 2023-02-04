#pragma once

namespace Engine {
namespace Execution {

    template <typename F, typename V, typename R>
    void detach(Sender<F, V, R> &&sender)
    {
        struct state {
            state(Sender<F, V, R> &&sender)
                : mRec { [](state *s) { delete s; }, this }
                , mState(sender(mRec))
            {
            }
            void start()
            {
                mState.start();
            }
            struct DeleteRec {
                void set_value(R, V)
                {
                    mDeleter(mState);
                }
                void set_done()
                {
                    mDeleter(mState);
                }
                void set_error(R)
                {
                    mDeleter(mState);
                }
                void (*mDeleter)(state *);
                state *mState;
            } mRec;
            std::invoke_result_t<F, DeleteRec &> mState;
        };
        (new state { std::move(sender) })->start();
    }

}
}
