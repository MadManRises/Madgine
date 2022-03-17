#pragma once

namespace Engine {
namespace Serialize {

    template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__serialize_impl__::QueryImpl<f, OffsetPtr, R, T, _Ty...>, Configs...> {

        static void writeAction(const Query<f, OffsetPtr> &query, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            assert(outStreams.size() == 1);
            for (FormattedBufferedStream &out : outStreams) {
                write(out, *static_cast<const patch_void_t<R> *>(data), nullptr, hierarchy);
                out.endMessageWrite();
            }
        }

        static StreamResult readAction(Query<f, OffsetPtr> &query, FormattedBufferedStream &in, PendingRequest *request, const CallerHierarchyBasePtr &hierarchy = {})
        {
            patch_void_t<R> returnValue;
            STREAM_PROPAGATE_ERROR(read(in, returnValue, nullptr, hierarchy));
            STREAM_PROPAGATE_ERROR(applyMap(in, returnValue, true));
            assert(request);
            query.writeRequestResponse(&returnValue, request->mRequester, request->mRequesterTransactionId);
            (*request)(&returnValue);
            return {};
        }

        static void writeRequest(const Query<f, OffsetPtr> &query, FormattedBufferedStream &out, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, hierarchy); });
            out.endMessageWrite();
        }

        static StreamResult readRequest(Query<f, OffsetPtr> &query, FormattedBufferedStream &in, TransactionId id, const CallerHierarchyBasePtr &hierarchy = {})
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                data, [&](auto &field, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return read(in, field, nullptr, hierarchy);
                },
                StreamResult {}));
            STREAM_PROPAGATE_ERROR(applyMap(in, data, true));
            query.tryCall(data, in.id(), id);
            return {};
        }
    };

}
}