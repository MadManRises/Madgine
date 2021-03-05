#pragma once

#include "action.h"
#include "../configs/configselector.h"
#include "../configs/verifier.h"
#include "../configs/requestpolicy.h"
#include "../streams/pendingrequest.h"
#include "../streams/bufferedstream.h"
#include "../unithelper.h"

namespace Engine {
namespace Serialize {




    template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__action__impl__::ActionImpl<f, OffsetPtr, R, T, _Ty...>, Configs...> {

        using RequestPolicy = ConfigSelectorDefault<RequestPolicyCategory, RequestPolicy::all_requests, Configs...>;
        using Verifier = ConfigSelectorDefault<VerifierCategory, DefaultVerifier, Configs...>;

        template <typename... Args>
        static void writeAction(const Action<f, OffsetPtr> &action, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data, Args &&... args)
        {
            for (BufferedOutStream *out : outStreams) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(*out, field, nullptr, args...); });
                out->endMessage();
            }
        }

        template <typename... Args>
        static StreamResult readAction(Action<f, OffsetPtr> &action, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            TupleUnpacker::forEach(data, [&](auto &field) { read(in, field, nullptr, args...); }); //TODO
            UnitHelper<decltype(data)>::applyMap(in, data);
            R result = action.call(std::move(data), request ? request->mRequester : 0, request ? request->mRequesterTransactionId : 0);
            if (request) {
                (*request)(&result);
            }
            return {};
        }

        template <typename... Args>
        static void writeRequest(const Action<f, OffsetPtr> &action, BufferedOutStream &out, const void *data, Args &&... args)
        {
            if constexpr (!RequestPolicy::sCallByMasterOnly) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, args...); });
                out.endMessage();
            } else {
                throw 0;
            }
        }

        template <typename... Args>
        static StreamResult readRequest(Action<f, OffsetPtr> &action, BufferedInOutStream &in, TransactionId id, Args &&... args)
        {
            if constexpr (!RequestPolicy::sCallByMasterOnly) {
                std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
                TupleUnpacker::forEach(data, [&](auto &field) { read(in, field, nullptr, args...); }); //TODO
                UnitHelper<decltype(data)>::applyMap(in, data);
                if (Verifier::verify(id, data)) {
                    action.tryCall(data, in.id(), id);
                } else {
                    return STREAM_PERMISSION_ERROR(in, "Request for action not verified");
                }
            } else {
                return STREAM_PERMISSION_ERROR(in, "Request for action not allowed");
            }
            return {};
        }
    };

}
}