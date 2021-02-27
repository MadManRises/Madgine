#pragma once

#include "../streams/bufferedstream.h"
#include "../streams/operations.h"
#include "../syncable.h"
#include "Generic/future.h"
#include "Generic/offsetptr.h"
#include "Generic/onetimefunctor.h"
#include "../unithelper.h"

namespace Engine {
namespace Serialize {
    namespace __action__impl__ {
        template <bool callByMasterOnly>
        struct _ActionPolicy {
            static constexpr bool sCallByMasterOnly = callByMasterOnly;

            template <typename... Args>
            static bool verify(ParticipantId requester, Args &&... args)
            {
                return true;
            }
        };

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
                    return call(args, requester, requesterTransactionId, targets);
                } else {
                    assert(targets.empty());
                    std::promise<R> p;
                    Future<R> fut { p.get_future() };
                    this->writeRequest(&args, requester, requesterTransactionId, oneTimeFunctor([p { std::move(p) }](void *data) mutable { p.set_value(*static_cast<R *>(data)); }));
                    return fut;
                }
            }
        };
    }

    struct ActionPolicy {
        using broadcast = __action__impl__::_ActionPolicy<false>;
        using notification = __action__impl__::_ActionPolicy<true>;
    };

    /*template <typename F, F f, class C>
		using Action = typename MemberFunctionCapture<__actionpolicy__impl__::ActionImpl, F, f, C>::type;*/

    template <auto f, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using Action = typename MemberFunctionCapture<__action__impl__::ActionImpl, f, OffsetPtr>::type;

#define ACTION(Name, f) OFFSET_CONTAINER(Name, ::Engine::Serialize::Action<&Self::f>)

    template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__action__impl__::ActionImpl<f, OffsetPtr, R, T, _Ty...>, Configs...> {
        using Config = type_pack_first_t<type_pack<Configs...>>;

        template <typename... Args>
        static void writeAction(const Action<f, OffsetPtr> &action, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data, Args &&... args)
        {
            for (BufferedOutStream *out : outStreams) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(*out, field, nullptr, args...); });
                out->endMessage();
            }
        }

        template <typename... Args>
        static void readAction(Action<f, OffsetPtr> &action, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            TupleUnpacker::forEach(data, [&](auto &field) { read(in, field, nullptr, args...); });
            UnitHelper<decltype(data)>::applyMap(in, data);
            R result = action.call(std::move(data), request ? request->mRequester : 0, request ? request->mRequesterTransactionId : 0);
            if (request) {
                (*request)(&result);
            }
        }

        template <typename... Args>
        static void writeRequest(const Action<f, OffsetPtr> &action, BufferedOutStream &out, const void *data, Args &&... args)
        {
            if constexpr (!Config::sCallByMasterOnly) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, args...); });
                out.endMessage();
            } else {
                throw 0;
            }
        }

        template <typename... Args>
        static void readRequest(Action<f, OffsetPtr> &action, BufferedInOutStream &in, TransactionId id, Args &&... args)
        {
            if constexpr (!Config::sCallByMasterOnly) {
                std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
                TupleUnpacker::forEach(data, [&](auto &field) { read(in, field, nullptr, args...); });
                UnitHelper<decltype(data)>::applyMap(in, data);
                if (Config::verify(id, data)) {
                    action.tryCall(data, in.id(), id);
                } else {
                    throw 0;
                }
            } else {
                throw 0;
            }
        }
    };

}
}
