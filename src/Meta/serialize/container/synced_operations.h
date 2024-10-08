#pragma once

namespace Engine {
namespace Serialize {

    template <typename T, typename Observer, typename OffsetPtr>
    struct Operations<Synced<T, Observer, OffsetPtr>> {        
        static StreamResult readRequest(Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &inout, MessageId id, const CallerHierarchyBasePtr &hierarchy = {})
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
                out.endMessageWrite();
            }
            return {};
        }

        static void writeRequest(const Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &out, const void *_data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> &data = *static_cast<const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> *>(_data);
            Serialize::write(out, data.first, nullptr);
            Serialize::write(out, data.second, nullptr);
            out.endMessageWrite();
        }

        static StreamResult readAction(Synced<T, Observer, OffsetPtr> &synced, FormattedBufferedStream &in, PendingRequest &request, const CallerHierarchyBasePtr &hierarchy = {})
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

        static void writeAction(const Synced<T, Observer, OffsetPtr> &synced, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *_data, const CallerHierarchyBasePtr &hierarchy = {})
        {
            const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> &data = *static_cast<const std::pair<Synced<T, Observer, OffsetPtr>::Operation, T> *>(_data);
            for (FormattedBufferedStream &out : outStreams) {
                Serialize::write(out, data.first, nullptr);
                Serialize::write(out, data.second, nullptr);
                out.endMessageWrite();
            }
        }

        static StreamResult read(FormattedSerializeStream &in, Synced<T, Observer, OffsetPtr> &synced, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            T old = synced.mData;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, synced.mData, name));
            synced.notify(old);
            return {};
        }

        static void write(FormattedSerializeStream &out, const Synced<T, Observer, OffsetPtr> &synced, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            Serialize::write(out, synced.mData, name);
        }

        static void setSynced(Synced<T, Observer, OffsetPtr> &synced, bool b)
        {
            Serialize::setSynced(synced.mData, b);
        }

        static void setActive(Synced<T, Observer, OffsetPtr> &synced, bool active, bool existenceChanged)
        {
            if (!active) {
                if (synced.mData != T {})
                    synced.Observer::operator()(T {}, synced.mData);
            }
            Serialize::setActive(synced.mData, active, existenceChanged);
            if (active) {
                if (synced.mData != T {})
                    synced.Observer::operator()(synced.mData, T {});
            }
        }

        static StreamResult applyMap(FormattedSerializeStream &in, Synced<T, Observer, OffsetPtr> &synced, bool success)
        {
            return Serialize::applyMap(in, synced.mData, success);
        }

        static void setParent(Synced<T, Observer, OffsetPtr> &synced, SerializableUnitBase *parent)
        {
            Serialize::setParent(synced.mData, parent);
        }

    };

}
}