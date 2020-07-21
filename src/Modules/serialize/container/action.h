#pragma once

#include "../../generic/offsetptr.h"
#include "../streams/bufferedstream.h"
#include "../streams/operations.h"
#include "../syncable.h"

namespace Engine {
namespace Serialize {
    namespace __action__impl__ {
        template <bool executeOnMasterOnly, bool callByMasterOnly>
        struct _ActionPolicy {
            static constexpr bool sExecuteOnMasterOnly = executeOnMasterOnly;
            static constexpr bool sCallByMasterOnly = callByMasterOnly;
        };

        template <auto f, typename Config, typename OffsetPtr, typename R, typename T, typename... _Ty>
        struct ActionImpl : Syncable<OffsetPtr> {

            friend struct Operations<ActionImpl<f, Config, OffsetPtr, R, T, _Ty...>>;

            ActionImpl()
            {
            }

            void operator()(_Ty... args, const std::set<ParticipantId> &targets = {})
            {
                tryCall(this->participantId(), targets, args...);
            }

            void setVerify(std::function<bool(ParticipantId, _Ty...)> verify)
            {
                mVerify = verify;
            }

        protected:
            bool verify(ParticipantId id, _Ty... args)
            {
                return !mVerify || mVerify(id, args...);
            }

        private:
            void call(const std::set<ParticipantId> &targets, _Ty... args)
            {
                if (!Config::sExecuteOnMasterOnly) {
                    std::tuple<_Ty...> arg_tuple { args... };
                    this->writeAction(0, &arg_tuple);
                } else {
                    assert(targets.empty());
                }

                (OffsetPtr::parent(this)->*f)(args...);
            }

            void tryCall(ParticipantId id, const std::set<ParticipantId> &targets, _Ty... args)
            {
                if (verify(id, args...)) {
                    if (this->isMaster()) {
                        call(targets, args...);
                    } else {
                        if (!Config::sCallByMasterOnly && targets.empty()) {
                            std::tuple<_Ty...> arg_tuple { args... };
                            this->writeRequest(0, &arg_tuple);
                        } else {
                            std::terminate();
                        }
                    }
                }
            }

        private:
            std::function<bool(ParticipantId, _Ty...)> mVerify;
        };
    }

    struct ActionPolicy {
        //using masterOnly = _ActionPolicy<true, true>;
        using request = __action__impl__::_ActionPolicy<true, false>;
        using broadcast = __action__impl__::_ActionPolicy<false, false>;
        using notification = __action__impl__::_ActionPolicy<false, true>;
    };

    /*template <typename F, F f, class C>
		using Action = typename MemberFunctionCapture<__actionpolicy__impl__::ActionImpl, F, f, C>::type;*/

    template <auto f, typename C, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using Action = typename MemberFunctionCapture<__action__impl__::ActionImpl, f, C, OffsetPtr>::type;

#define ACTION(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::Action<__VA_ARGS__>)

    template <auto f, typename Config, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__action__impl__::ActionImpl<f, Config, OffsetPtr, R, T, _Ty...>, Configs...> {
        template <typename... Args>
        static void writeAction(const Action<f, Config, OffsetPtr> &action, int op, const void *data, ParticipantId answerTarget, TransactionId answerId, Args &&... args)
        {
            for (BufferedOutStream *out : action.getMasterActionMessageTargets(answerTarget, answerId)) {
                *out << *static_cast<const std::tuple<_Ty...> *>(data);
                out->endMessage();
            }
        }

        template <typename... Args>
        static void readAction(Action<f, Config, OffsetPtr> &action, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            in >> data;
            UnitHelper<decltype(data)>::applyMap(in, data);
            TupleUnpacker::invokeExpand(&Action<f, Config, OffsetPtr>::call, action, std::set<ParticipantId> {}, std::move(data));
        }

        template <typename... Args>
        static void writeRequest(const Action<f, Config, OffsetPtr> &action, int op, const void *data, ParticipantId requesterId, TransactionId requesterTransactionId, std::function<void(void *)> callback, Args &&... args)
        {
            BufferedOutStream *out = action.getSlaveActionMessageTarget(requesterId, requesterTransactionId, std::move(callback));
            *out << *static_cast<const std::tuple<_Ty...> *>(data);
            out->endMessage();
        }

        template <typename... Args>
        static void readRequest(Action<f, Config, OffsetPtr> &action, BufferedInOutStream &in, TransactionId id, Args &&... args)
        {
            if (!Config::sCallByMasterOnly) {
                std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
                in >> data;
                UnitHelper<decltype(data)>::applyMap(in, data);
                TupleUnpacker::invokeExpand(&Action<f, Config, OffsetPtr>::tryCall, action, in.id(), std::set<ParticipantId> {}, std::move(data));
            }
        }
    };

}
}
