#pragma once

#include "../genericresult.h"
#include "virtualsender.h"
#include "virtualstate.h"

namespace Engine {
namespace Execution {

    template <typename Stub, typename... Ty>
    struct Connection : VirtualReceiverBase<GenericResult, Ty...> {

        Connection(Stub *stub)
            : mStub(stub)
        {
        }

        void start()
        {
            mStub->enqueue(this);
        }

    protected:
        template <typename>
        friend struct ConnectionStack;
        template <typename>
        friend struct ConnectionQueue;

        Stub *mStub;
        std::atomic<Connection<Stub, Ty...> *> mNext = nullptr;
    };

    template <typename Stub, typename Rec, typename... Ty>
    struct StoppableConnection : VirtualStateEx<Rec, Connection<Stub, Ty...>, type_pack<GenericResult>, Ty...> {

        struct callback {
            callback(StoppableConnection<Stub, Rec, Ty...> *con)
                : mCon(con)
            {
            }

            void operator()()
            {
                mCon->stop();
            }

            StoppableConnection<Stub, Rec, Ty...> *mCon;
        };

        StoppableConnection(Rec &&rec, Stub *stub)
            : Execution::VirtualStateEx<Rec, Connection<Stub, Ty...>, type_pack<GenericResult>, Ty...>(std::forward<Rec>(rec), stub)
            , mCallback(Execution::get_stop_token(this->mRec), callback { this })
        {
        }

        void start()
        {
            if (Execution::get_stop_token(this->mRec).stop_requested())
                this->set_done();
            else
                Connection<Stub, Ty...>::start();
        }

        void stop()
        {
            if (this->mStub->extract(this))
                this->set_done();
        }

        std::stop_callback<callback> mCallback;
    };

    template <typename Stub, typename... Ty>
    struct ConnectionSender {

        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<Ty...>;

        using is_sender = void;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, ConnectionSender<Stub, Ty...> &&sender, Rec &&rec)
        {
            if constexpr (Execution::has_stop_token<Rec>)
                return StoppableConnection<Stub, Rec, Ty...> { std::forward<Rec>(rec), sender.mStub };
            else
                return Execution::make_virtual_state<Connection<Stub, Ty...>, GenericResult, Ty...>(std::forward<Rec>(rec), sender.mStub);
        }

        Stub *mStub;
    };

}
}
