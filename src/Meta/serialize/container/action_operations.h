#pragma once

#include "../configs/verifier.h"

namespace Engine {
namespace Serialize {

    template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__serialize_impl__::ActionImpl<f, OffsetPtr, R, T, _Ty...>, Configs...> {

        using RequestPolicy = ConfigSelectorDefault<RequestPolicyCategory, RequestPolicy::all_requests, Configs...>;
        using Verifier = ConfigSelectorDefault<VerifierCategory, DefaultVerifier, Configs...>;

        static void writeAction(const Action<f, OffsetPtr> &action, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            for (FormattedBufferedStream &out : outStreams) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, hierarchy); });
                out.endMessageWrite();
            }
        }

        static StreamResult readAction(Action<f, OffsetPtr> &action, FormattedBufferedStream &in, PendingRequest *request, const CallerHierarchyBasePtr &hierarchy = {})
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                data, [&](auto &field, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return read(in, field, nullptr, hierarchy);
                },
                StreamResult {}));
            STREAM_PROPAGATE_ERROR(applyMap(in, data, true));

            auto result = action.call(std::move(data), request ? request->mRequester : 0, request ? request->mRequesterTransactionId : 0);
            if (request) {
                (*request)(&result);
            }

            return {};
        }

        static void writeRequest(const Action<f, OffsetPtr> &action, FormattedBufferedStream &out, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            if constexpr (!RequestPolicy::sCallByMasterOnly) {
                TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, hierarchy); });
                out.endMessageWrite();
            } else {
                throw 0;
            }
        }

        static StreamResult readRequest(Action<f, OffsetPtr> &action, FormattedBufferedStream &in, TransactionId id, const CallerHierarchyBasePtr &hierarchy = {})
        {
            if constexpr (!RequestPolicy::sCallByMasterOnly) {
                std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
                STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                    data, [&](auto &field, StreamResult r) {
                        STREAM_PROPAGATE_ERROR(std::move(r));
                        return read(in, field, nullptr, hierarchy);
                    },
                    StreamResult {}));
                STREAM_PROPAGATE_ERROR(applyMap(in, data, true));
                if (!TupleUnpacker::invokeExpand(Verifier::verify, OffsetPtr::parent(&action), in.id(), data))
                    return STREAM_PERMISSION_ERROR(in) << "Request for action not verified";
                action.tryCall(data, in.id(), id);
                return {};
            } else {
                return STREAM_PERMISSION_ERROR(in) << "Request for action not allowed";
            }
        }
    };

}
}