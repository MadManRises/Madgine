#pragma once

#include "query.h"

namespace Engine {
namespace Serialize {

   template <auto f, typename OffsetPtr, typename R, typename T, typename... _Ty, typename... Configs>
    struct Operations<__query__impl__::QueryImpl<f, OffsetPtr, R, T, _Ty...>, Configs...> {

        template <typename... Args>
        static void writeAction(const Query<f, OffsetPtr> &query, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data, Args &&... args)
        {
            assert(outStreams.size() == 1);
            for (BufferedOutStream *out : outStreams) {
                write(*out, *static_cast<const R *>(data), nullptr, args...);
                out->endMessage();
            }
        }

        template <typename... Args>
        static void readAction(Query<f, OffsetPtr> &query, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            R result;
            read(in, result, nullptr, args...);
            UnitHelper<R>::applyMap(in, result);
            assert(request);
            query.writeActionResponse(&result, request->mRequester, request->mRequesterTransactionId);
            (*request)(&result);
        }

        template <typename... Args>
        static void writeRequest(const Query<f, OffsetPtr> &query, BufferedOutStream &out, const void *data, Args &&... args)
        {
            TupleUnpacker::forEach(*static_cast<const std::tuple<_Ty...> *>(data), [&](auto &field) { write(out, field, nullptr, args...); });
            out.endMessage();
        }

        template <typename... Args>
        static void readRequest(Query<f, OffsetPtr> &query, BufferedInOutStream &in, TransactionId id, Args &&... args)
        {
            std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> data;
            TupleUnpacker::forEach(data, [&](auto &field) { read(in, field, nullptr, args...); });
            UnitHelper<decltype(data)>::applyMap(in, data);
            query.tryCall(data, in.id(), id);
        }
    };


}
}