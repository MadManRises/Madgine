#pragma once

namespace Engine {
namespace Serialize {

	template <typename T, typename Observer, typename OffsetPtr>
    struct Operations<Synced<T, Observer, OffsetPtr>> {
        template <typename... Args>
        static StreamResult readRequest(Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &inout, TransactionId id, Args &&...args)
        {
            if (synced.isMaster()) {
                typename Synced<T, Observer, OffsetPtr>::Operation op;
                STREAM_PROPAGATE_ERROR(Serialize::read(inout, op, nullptr));
                T old = synced.mData;
                T value;
                STREAM_PROPAGATE_ERROR(Serialize::read(inout, value, nullptr));
                switch (op) {
                case Synced<T, Observer, OffsetPtr>::Operation::SET:
                    synced.mData = value;
                    break;
                case Synced<T, Observer, OffsetPtr>::Operation::ADD:
                    if constexpr (has_function_PlusAssign_v<T>)
                        synced.mData += value;
                    else
                        throw 0;
                    break;
                case Synced<T, Observer, OffsetPtr>::Operation::SUB:
                    if constexpr (has_function_MinusAssign_v<T>)
                        synced.mData -= value;
                    else
                        throw 0;
                    break;
                }
                synced.notify(old, inout.id(), id);
            } else {
                FormattedBufferedStream &out = synced.getSlaveRequestMessageTarget(inout.id(), id);
                out.stream().pipe(inout.stream());
                out.endMessage();
            }
            return {};
        }

        template <typename... Args>
        static void writeRequest(const Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &out, const void *_data, Args &&...args)
        {
            const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> &data = *static_cast<const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> *>(_data);
            Serialize::write(out, data.first, nullptr);
            Serialize::write(out, data.second, nullptr);
            out.endMessage();
        }

        template <typename... Args>
        static StreamResult readAction(Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &in, PendingRequest *request, Args &&...args)
        {
            typename Synced<T, Observer, OffsetPtr>::Operation op;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, op, nullptr));
            T old = synced.mData;
            T value;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, value, nullptr));
            switch (op) {
            case Synced<T, Observer, OffsetPtr>::Operation::SET:
                synced.mData = value;
                break;
            case Synced<T, Observer, OffsetPtr>::Operation::ADD:
                if constexpr (has_function_PlusAssign_v<T>)
                    synced.mData += value;
                else
                    throw 0;
                break;
            case Synced<T, Observer, OffsetPtr>::Operation::SUB:
                if constexpr (has_function_MinusAssign_v<T>)
                    synced.mData -= value;
                else
                    throw 0;
                break;
            }
            synced.notify(old);
            return {};
        }

        template <typename... Args>
        static void writeAction(const Synced<T, Observer, OffsetPtr> &synced, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *_data, Args &&...args)
        {
            const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> &data = *static_cast<const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> *>(_data);
            for (FormattedBufferedStream &out : outStreams) {
                Serialize::write(out, data.first, nullptr);
                Serialize::write(out, data.second, nullptr);
                out.endMessage();
            }
        }

        template <typename... Args>
        static StreamResult read(FormattedSerializeStream &in, Synced<T, Observer, OffsetPtr> &synced, const char *name, Args &&...args)
        {
            T old = synced.mData;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, synced.mData, name));
            synced.notify(old);
            return {};
        }

        template <typename... Args>
        static void write(FormattedSerializeStream &out, const Synced<T, Observer, OffsetPtr> &synced, const char *name, Args &&...args)
        {
            Serialize::write(out, synced.mData, name);
        }
    };

}
}