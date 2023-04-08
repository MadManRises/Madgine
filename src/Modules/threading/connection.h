#pragma once

#include "Generic/execution/virtualreceiver.h"
#include "Generic/genericresult.h"

namespace Engine {
namespace Threading {

    template <typename... Ty>
    struct Connection : protected Execution::VirtualReceiverBase<GenericResult, Ty...> {

        Connection(SignalStub<Ty...> *stub)
            : mStub(stub)
        {
        }

        void start()
        {
            mStub->enqueue(this, mNext);
        }

        void cancel(Connection *&next)
        {
            next = mNext;
            mNext = nullptr;

            this->set_done();
        }

        void cancel(Connection *con, Connection *&next)
        {
            if (con == this) {
                cancel(next);
            } else {
                mNext->cancel(con, mNext);
            }
        }

        Connection *signal(Ty... args)
        {
            Connection *next = mNext;
            mNext = nullptr;

            this->set_value(std::forward<Ty>(args)...);

            return next;
        }

        Connection<Ty...> *next() const
        {
            return mNext;
        }

    protected:
        SignalStub<Ty...> *mStub;
        Connection<Ty...> *mNext = nullptr;
    };

    template <typename... Ty>
    struct StoppableConnection : Connection<Ty...> {

        struct callback {
            callback(StoppableConnection<Ty...> *con)
                : mCon(con)
            {
            }

            void operator()()
            {
                mCon->stop();
            }

            StoppableConnection<Ty...> *mCon;
        };

        StoppableConnection(SignalStub<Ty...> *stub, std::stop_token stop)
            : Connection<Ty...>(stub)
            , mCallback(std::move(stop), this)
        {
        }

        void start()
        {
            if (mStopped.test())
                this->set_done();
            else
                Connection<Ty...>::start();
        }

        void stop()
        {
            bool before = mStopped.test_and_set();
            assert(!before);
            mStub->disconnect(this);
        }

        std::stop_callback<callback> mCallback;
        std::atomic_flag mStopped = ATOMIC_FLAG_INIT;
    };

}
}
