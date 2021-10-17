#pragma once

#include "../syncable.h"
#include "Generic/future.h"
#include "Generic/memberoffsetptr.h"

namespace Engine {
namespace Serialize {
    namespace __action__impl__ {

        template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty>
        struct ActionImpl : Syncable<OffsetPtr> {

            template <typename, typename... Configs>
            friend struct Serialize::Operations;

            Future<R> operator()(_Ty... args, const std::set<ParticipantId> &targets = {})
            {
                return tryCall({ args... }, 0, 0, targets);
            }

        private:
            R call(std::tuple<_Ty...> args, ParticipantId answerTarget = 0, TransactionId answerId = 0, const std::set<ParticipantId> &targets = {})
            {
                this->writeAction(&args, answerTarget, answerId, targets);

                return TupleUnpacker::invokeExpand(f, OffsetPtr::parent(this), args);
            }

            Future<R> tryCall(std::tuple<_Ty...> args, ParticipantId requester = 0, TransactionId requesterTransactionId = 0, const std::set<ParticipantId> &targets = {})
            {
                if (this->isMaster()) {
                    if constexpr (std::is_same_v<R, void>) {
                        call(args, requester, requesterTransactionId, targets);
                        return make_ready_future();
                    } else {
                        return call(args, requester, requesterTransactionId, targets);
                    }
                } else {
                    assert(targets.empty());
                    std::promise<R> p;
                    Future<R> fut { p.get_future() };
                    this->writeRequest(&args, requester, requesterTransactionId, [p { std::move(p) }](void *data) mutable {
                        if constexpr (std::is_same_v<R, void>) {
                            p.set_value();
                        } else {
                            p.set_value(*static_cast<R *>(data));
                        }
                    });
                    return fut;
                }
            }
        };
    }

    template <auto f, typename OffsetPtr = TaggedPlaceholder<MemberOffsetPtrTag, 0>>
    using Action = typename FunctionCapture<__action__impl__::ActionImpl, f, OffsetPtr>::type;

#define ACTION(Name, f) MEMBER_OFFSET_CONTAINER(Name, ::Engine::Serialize::Action<&Self::f>)

}
}
