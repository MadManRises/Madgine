#pragma once

#include "../syncable.h"
#include "Generic/future.h"
#include "Generic/offsetptr.h"
#include "Generic/onetimefunctor.h"

namespace Engine {
namespace Serialize {
    namespace __query__impl__ {

        template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty>
        struct QueryImpl : Syncable<OffsetPtr> {

            template <typename, typename... Configs>
            friend struct Serialize::Operations;

            Future<R> operator()(_Ty... args)
            {
                return tryCall({ args... });
            }

        private:
            R call(std::tuple<_Ty...> args, ParticipantId answerTarget = 0, TransactionId answerId = 0)
            {
                R result = TupleUnpacker::invokeExpand(f, OffsetPtr::parent(this), args);

                this->writeRequestResponse(&result, answerTarget, answerId);

                return result;
            }

            Future<R> tryCall(std::tuple<_Ty...> args, ParticipantId requester = 0, TransactionId requesterTransactionId = 0)
            {
                if (this->isMaster()) {
                    return call(args, requester, requesterTransactionId);
                } else {
                    std::promise<R> p;
                    Future<R> fut { p.get_future() };
                    this->writeRequest(&args, requester, requesterTransactionId, oneTimeFunctor([p { std::move(p) }](void *data) mutable { p.set_value(*static_cast<R *>(data)); }));
                    return fut;
                }
            }
        };
    }

    template <auto f, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using Query = typename FunctionCapture<__query__impl__::QueryImpl, f, OffsetPtr>::type;

#define QUERY(Name, f) OFFSET_CONTAINER(Name, ::Engine::Serialize::Query<&Self::f>)

 
}
}
