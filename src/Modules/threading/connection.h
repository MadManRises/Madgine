#pragma once

#include "Generic/execution/virtualstate.h"
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
            } else if (mNext) {
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

    template <typename Rec, typename... Ty>
    struct StoppableConnection : Execution::VirtualStateEx<Rec, Connection<Ty...>, type_pack<GenericResult>, Ty...> {

        struct callback {
            callback(StoppableConnection<Rec, Ty...> *con)
                : mCon(con)
            {
            }

            void operator()()
            {
                mCon->stop();
            }

            StoppableConnection<Rec, Ty...> *mCon;
        };

        StoppableConnection(Rec &&rec, SignalStub<Ty...> *stub)
            : Execution::VirtualStateEx<Rec, Connection<Ty...>, type_pack<GenericResult>, Ty...>(std::forward<Rec>(rec), stub)
            , mCallback(Execution::get_stop_token(this->mRec), callback { this })
        {
        }

        void start()
        {
            if (Execution::get_stop_token(this->mRec).stop_requested())
                this->set_done();
            else
                Connection<Ty...>::start();
        }

        void stop()
        {
            this->mStub->disconnect(this);
        }

        std::stop_callback<callback> mCallback;
    };

    template <typename... Ty>
    struct ConnectionSender {

        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<Ty...>;

        using is_sender = void;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, ConnectionSender<Ty...> &&sender, Rec &&rec)
        {
            if constexpr (Execution::has_stop_token<Rec>)
                return StoppableConnection<Rec, Ty...> { std::forward<Rec>(rec), sender.mStub };
            else
                return Execution::make_virtual_state<Connection<Ty...>, GenericResult, Ty...>(std::forward<Rec>(rec), sender.mStub);
        }

        SignalStub<Ty...> *mStub;
    };

}
}
