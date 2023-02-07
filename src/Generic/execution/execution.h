#pragma once

namespace Engine {
namespace Execution {

    template <typename S, typename V, typename R>
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
        void (*mDeleter)(S *);
        S *mState;
    };
        
    template <typename S, typename R>
    struct DeleteRec<S, void, R> {
        void set_value(R)
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
        void (*mDeleter)(S *);
        S *mState;
    };

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
            DeleteRec<state, V, R> mRec;
            std::invoke_result_t<F, DeleteRec<state, V, R> &> mState;
        };
        (new state { std::move(sender) })->start();
    }

}
}
