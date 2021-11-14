#pragma once

#include "../configs/configselector.h"
#include "../configs/creator.h"
#include "../configs/requestpolicy.h"
#include "../container/physical.h"
#include "../formatter.h"
#include "../primitivetypes.h"
#include "../serializableunitptr.h"
#include "Generic/callerhierarchy.h"
#include "Generic/container/atomiccontaineroperation.h"
#include "Generic/container/containerevent.h"
#include "Generic/indextype.h"
#include "bufferedstream.h"
#include "comparestreamid.h"
#include "pendingrequest.h"

namespace Engine {
namespace Serialize {

    DERIVE_TYPENAME(Config);
    DERIVE_FUNCTION(writeState, SerializeOutStream &, const char *)
    DERIVE_FUNCTION2(writeState2, writeState, SerializeOutStream &, const char *, CallerHierarchyBasePtr)
    DERIVE_FUNCTION(readState, SerializeInStream &, const char *)
    DERIVE_FUNCTION2(readState2, readState, SerializeInStream &, const char *, CallerHierarchyBasePtr)
    /*DERIVE_FUNCTION(readAction, SerializeInStream &, PendingRequest*)
    DERIVE_FUNCTION(readRequest, BufferedInOutStream &, TransactionId)
    DERIVE_FUNCTION(writeAction, int, const void *, ParticipantId, TransactionId)
    DERIVE_FUNCTION(writeRequest, int, const void *, ParticipantId, TransactionId, std::function<void(void*)>)*/

    template <typename C, typename... Configs>
    struct ContainerOperations {

        using Creator = ConfigSelectorDefault<CreatorCategory, DefaultCreator<typename C::value_type>, Configs...>;
        using RequestPolicy = ConfigSelectorDefault<RequestPolicyCategory, RequestPolicy::all_requests, Configs...>;

        template <typename Op, typename Hierarchy = std::monostate>
        static StreamResult readOp(SerializeInStream &in, Op &op, const char *name, const Hierarchy &hierarchy = {})
        {
            using T = typename C::value_type;

            STREAM_PROPAGATE_ERROR(in.format().beginContainer(in, name, !container_traits<C>::is_fixed_size));

            if constexpr (!container_traits<C>::is_fixed_size) {
                TupleUnpacker::invoke(&Creator::template clear<Op>, op, hierarchy);

                while (in.format().hasContainerItem(in)) {
                    typename container_traits<Op>::emplace_return it;
                    STREAM_PROPAGATE_ERROR(TupleUnpacker::invoke(&Creator::template readItem<Op>, in, op, it, physical(op).end(), hierarchy));
                }
            } else {
                for (T &t : physical(op)) {
                    STREAM_PROPAGATE_ERROR(Serialize::read(in, t, "Item"));
                }
            }

            return in.format().endContainer(in, name);
        }

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, C &container, const char *name, Args &&... args)
        {
            decltype(auto) op = resetOperation(container);
            return readOp(in, op, name, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const C &container, const char *name, Args &&... args)
        {
            if constexpr (container_traits<C>::is_fixed_size)
                out.format().beginContainer(out, name);
            else
                out.format().beginContainer(out, name, container.size());
            for (const auto &t : container) {
                TupleUnpacker::invoke(&Creator::writeItem, out, t, std::forward<Args>(args)...);
            }
            out.format().endContainer(out, name);
        }

        static StreamResult readIterator(SerializeInStream &in, C &c, typename C::iterator &it)
        {
            int32_t dist;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, dist, "it"));
            it = std::next(c.begin(), dist);
            return {};
        }

        static void writeIterator(SerializeOutStream &out, const C &c, const typename C::const_iterator &it)
        {
            Serialize::write<int32_t>(out, std::distance(c.begin(), it), "it");
        }

        template <typename... Args>
        static StreamResult performOperation(C &c, ContainerEvent op, SerializeInStream &in, typename container_traits<C>::emplace_return &it, ParticipantId answerTarget, TransactionId answerId, Args &&... args)
        {
            it = c.end();
            switch (op) {
            case EMPLACE: {
                if constexpr (!container_traits<C>::sorted) {
                    readIterator(in, c, it);
                }
                decltype(auto) op = insertOperation(c, it, answerTarget, answerId);
                typename container_traits<C>::const_iterator cit = static_cast<const typename container_traits<C>::iterator &>(it);
                return TupleUnpacker::invoke(&Creator::template readItem<InsertOperation_t<C>>, in, op, it, cit, std::forward<Args>(args)...);
                break;
            }
            case ERASE: {
                readIterator(in, c, it);
                it = removeOperation(c, it, answerTarget, answerId).erase(it);
                break;
            }
                /*case REMOVE_RANGE: {
                iterator from = this->read_iterator(in);
                iterator to = this->read_iterator(in);
                it = erase_impl(from, to, answerTarget, answerId);
                break;
            }
            case RESET:
                //readState_impl(in, nullptr, answerTarget, answerId);
                throw "TODO";
                break;*/
            default:
                throw 0;
            }
            return {};
        }

        //TODO: Maybe move loop out of this function
        template <typename... Args>
        static void writeAction(const C &c, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *_data, Args &&... args)
        {
            const auto &[op, it] = *static_cast<const std::tuple<ContainerEvent, typename C::const_iterator> *>(_data);

            for (BufferedOutStream *out : outStreams) {
                Serialize::write(*out, op, "op");
                switch (op) {
                case EMPLACE: {
                    if constexpr (!container_traits<C>::sorted) {
                        writeIterator(*out, c, it);
                    }
                    TupleUnpacker::invoke(&Creator::writeItem, *out, *it, std::forward<Args>(args)...);
                    break;
                }
                case ERASE: {
                    writeIterator(*out, c, it);
                    break;
                }
                default:
                    throw 0;
                }
                out->endMessage();
            }
        }

        template <typename... Args>
        static StreamResult readAction(C &c, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            ContainerEvent op;
            in >> op;

            bool accepted = (op & ~MASK) != ABORTED;

            if (accepted) {
                typename container_traits<C>::emplace_return it;
                STREAM_PROPAGATE_ERROR(performOperation(c, op, in, it, request ? request->mRequester : 0, request ? request->mRequesterTransactionId : 0, std::forward<Args>(args)...));
                if (request) {
                    (*request)(&it);
                }
            } else {
                assert(request);
                if (request->mRequesterTransactionId) {
                    BufferedOutStream &out = c.getRequestResponseTarget(request->mRequester, request->mRequesterTransactionId);
                    out << op;
                    out.endMessage();
                }
                (*request)(nullptr);
            }
            return {};
        }

        template <typename... Args>
        static void writeRequest(const C &c, BufferedOutStream &out, const void *_data, Args &&... args)
        {
            if (RequestPolicy::sCallByMasterOnly)
                throw 0;

            const auto &[op, it, item] = *static_cast<const std::tuple<ContainerEvent, typename C::const_iterator, typename C::value_type &> *>(_data);
            out << op;
            switch (op) {
            case EMPLACE: {
                if constexpr (!container_traits<C>::sorted) {
                    writeIterator(out, c, it);
                }
                TupleUnpacker::invoke(&Creator::writeItem, out, item, std::forward<Args>(args)...);
                break;
            }
            default:
                throw 0;
            }
            out.endMessage();
        }

        template <typename... Args>
        static StreamResult readRequest(C &c, BufferedInOutStream &inout, TransactionId id, Args &&... args)
        {
            bool accepted = !RequestPolicy::sCallByMasterOnly;

            ContainerEvent op;
            inout >> op;

            if (!accepted) {
                if (id) {
                    c.beginRequestResponseMessage(inout, id);
                    Serialize::write(inout, op | ABORTED, "op");
                    inout.endMessage();
                }
            } else {
                if (c.isMaster()) {
                    typename container_traits<C>::emplace_return it;
                    STREAM_PROPAGATE_ERROR(performOperation(c, op, inout, it, inout.id(), id, std::forward<Args>(args)...));
                } else {
                    BufferedOutStream &out = c.getSlaveActionMessageTarget(inout.id(), id);
                    Serialize::write(out, op, "op");
                    out.pipe(inout);
                    out.endMessage();
                }
            }
            return {};
        }
    };

    template <typename T, typename... Configs>
    struct Operations;

    template <typename T, typename... Configs>
    struct BaseOperations {
        template <typename Hierarchy = std::monostate>
        static StreamResult read(SerializeInStream &in, T &t, const char *name, const Hierarchy &hierarchy = {}, StateTransmissionFlags flags = 0)
        {
            if constexpr (has_function_readState2_v<T>) {
                return t.readState(in, name, CallerHierarchyPtr { hierarchy });
            } else if constexpr (has_function_readState_v<T>) {
                return t.readState(in, name);
            } else if constexpr (isPrimitiveType_v<T>) {
                STREAM_PROPAGATE_ERROR(in.format().beginPrimitive(in, name, PrimitiveTypeIndex_v<T>));
                STREAM_PROPAGATE_ERROR(in.readUnformatted(t));
                return in.format().endPrimitive(in, name, PrimitiveTypeIndex_v<T>);
                //mLog.log(t);
            } else if constexpr (std::is_const_v<T>) {
                //Don't do anything here
                return {};
            } else if constexpr (std::is_base_of_v<SerializableUnitBase, T>) {
                return SerializableUnitPtr { &t }.readState(in, name, CallerHierarchyPtr { hierarchy }, flags);
            } else if constexpr (std::is_base_of_v<SerializableDataUnit, T>) {
                return SerializableDataPtr { &t }.readState(in, name, CallerHierarchyPtr { hierarchy }, flags);
            } else if constexpr (is_string_like_v<T>) {
                std::string s;
                StreamResult result = Serialize::read<std::string>(in, s, name, hierarchy);
                t = s;
                return result;
            } else if constexpr (is_iterable_v<T>) {
                return ContainerOperations<T, Configs...>::read(in, t, name, hierarchy);
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                return Operations<decltype(TupleUnpacker::toTuple(std::declval<T &>())), Configs...>::read(in, TupleUnpacker::toTuple(t), name, hierarchy);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename Hierarchy = std::monostate>
        static void write(SerializeOutStream &out, const T &t, const char *name, const Hierarchy &hierarchy = {}, StateTransmissionFlags flags = 0)
        {
            if constexpr (has_function_writeState2_v<T>) {
                t.writeState(out, name, CallerHierarchyPtr { hierarchy });
            } else if constexpr (has_function_writeState_v<T>) {
                t.writeState(out, name);
            } else if constexpr (isPrimitiveType_v<T>) {
                out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
                out.writeUnformatted(t);
                out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
                //mLog.log(t);
            } else if constexpr (std::is_const_v<T>) {
                //Don't do anything here
            } else if constexpr (std::is_base_of_v<SerializableUnitBase, T>) {
                SerializableUnitConstPtr { &t }.writeState(out, name, CallerHierarchyPtr { hierarchy }, flags);
            } else if constexpr (std::is_base_of_v<SerializableDataUnit, T>) {
                SerializableDataConstPtr { &t }.writeState(out, name, CallerHierarchyPtr { hierarchy }, flags);
            } else if constexpr (is_string_like_v<T>) {
                Serialize::write<std::string>(out, t, name, hierarchy);
            } else if constexpr (is_iterable_v<T>) {
                ContainerOperations<std::remove_const_t<T>, Configs...>::write(out, t, name, hierarchy);
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                Operations<decltype(TupleUnpacker::toTuple(std::declval<T &>())), Configs...>::write(out, TupleUnpacker::toTuple(t), name, hierarchy);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename... Args>
        static void writeAction(const T &t, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data, Args &&... args)
        {
            /*if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (has_function_writeAction_v<T>) {
            t.writeAction(op, data, answerTarget, answerId);
        } else */
            if constexpr (is_iterable_v<T>) {
                ContainerOperations<T, Configs...>::writeAction(t, outStreams, data, std::forward<Args>(args)...);
            } /*else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } */
            else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename... Args>
        static StreamResult readAction(T &t, SerializeInStream &in, PendingRequest *request, Args &&... args)
        {
            /*if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (has_function_readAction_v<T>) {
             t.readAction(in, request);
        } else */
            if constexpr (is_iterable_v<T>) {
                return ContainerOperations<T, Configs...>::readAction(t, in, request, std::forward<Args>(args)...);
            } /*else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } */
            else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename... Args>
        static void writeRequest(const T &t, BufferedOutStream &out, const void *data, Args &&... args)
        {
            /*if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else  if constexpr (has_function_writeRequest_v<T>) {
            t.writeRequest(op, data, requester ,requesterTransactionId, callback);
        } else */
            if constexpr (is_iterable_v<T>) {
                ContainerOperations<T, Configs...>::writeRequest(t, out, data, std::forward<Args>(args)...);
            } /*else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } */
            else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename... Args>
        static StreamResult readRequest(T &t, BufferedInOutStream &inout, TransactionId id, Args &&... args)
        {
            /*if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (has_function_readRequest_v<T>) {
            t.readRequest(inout, id);
        } else */
            if constexpr (is_iterable_v<T>) {
                return ContainerOperations<T, Configs...>::readRequest(t, inout, id, std::forward<Args>(args)...);
            } /*else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } */
            else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }
    };

    template <typename T, typename... Configs>
    struct Operations : BaseOperations<T, Configs...> {
    };

    template <typename T, typename... Configs>
    struct Operations<std::unique_ptr<T>, Configs...> {

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, const std::unique_ptr<T> &p, const char *name, Args &&... args)
        {
            return Operations<T, Configs...>::read(in, *p, name, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const std::unique_ptr<T> &p, const char *name, Args &&... args)
        {
            Operations<T, Configs...>::write(out, *p, name, std::forward<Args>(args)...);
        }
    };

    template <typename T, typename... Configs>
    struct Operations<const std::unique_ptr<T>, Configs...> {

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, const std::unique_ptr<T> &p, const char *name, Args &&... args)
        {
            return Operations<T, Configs...>::read(in, *p, name, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const std::unique_ptr<T> &p, const char *name, Args &&... args)
        {
            Operations<T, Configs...>::write(out, *p, name, std::forward<Args>(args)...);
        }
    };

    template <typename... Ty, typename... Configs>
    struct Operations<std::tuple<Ty...>, Configs...> {

        template <typename... Args, size_t... Is>
        static StreamResult read(SerializeInStream &in, std::tuple<Ty...> &t, const char *name, std::index_sequence<Is...>, Args &&... args)
        {
            STREAM_PROPAGATE_ERROR(in.format().beginContainer(in, name, false));
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                t, [&](auto &e, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return Serialize::read(in, e, nullptr, args...);
                },
                StreamResult {}));
            return in.format().endContainer(in, name);
        }

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, std::tuple<Ty...> &t, const char *name, Args &&... args)
        {
            return read(in, t, name, std::make_index_sequence<sizeof...(Ty)> {}, std::forward<Args>(args)...);
        }

        template <typename... Args, size_t... Is>
        static void write(SerializeOutStream &out, const std::tuple<Ty...> &t, const char *name, std::index_sequence<Is...>, Args &&... args)
        {
            out.format().beginContainer(out, name);
            (Serialize::write<Ty>(out, std::get<Is>(t), "Element", args...), ...);
            out.format().endContainer(out, name);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const std::tuple<Ty...> &t, const char *name, Args &&... args)
        {
            write(out, t, name, std::make_index_sequence<sizeof...(Ty)> {}, std::forward<Args>(args)...);
        }
    };

    template <typename... Ty, typename... Configs>
    struct Operations<std::tuple<Ty &...>, Configs...> {

        template <typename... Args, size_t... Is>
        static StreamResult read(SerializeInStream &in, std::tuple<Ty &...> t, const char *name, std::index_sequence<Is...>, Args &&... args)
        {
            STREAM_PROPAGATE_ERROR(in.format().beginContainer(in, name, false));
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                t, [&](auto &e, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return Serialize::read(in, e, nullptr, args...);
                },
                StreamResult {}));
            return in.format().endContainer(in, name);
        }

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, std::tuple<Ty &...> t, const char *name, Args &&... args)
        {
            return read(in, t, name, std::make_index_sequence<sizeof...(Ty)> {}, std::forward<Args>(args)...);
        }

        template <typename... Args, size_t... Is>
        static void write(SerializeOutStream &out, const std::tuple<const Ty &...> t, const char *name, std::index_sequence<Is...>, Args &&... args)
        {
            out.format().beginContainer(out, name);
            (Serialize::write<Ty>(out, std::get<Is>(t), "Element", args...), ...);
            out.format().endContainer(out, name);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const std::tuple<const Ty &...> t, const char *name, Args &&... args)
        {
            write(out, t, name, std::make_index_sequence<sizeof...(Ty)> {}, std::forward<Args>(args)...);
        }
    };

    template <typename U, typename V, typename... Configs>
    struct Operations<std::pair<U, V>, Configs...> {

        template <typename... Args>
        static StreamResult read(SerializeInStream &in, std::pair<U, V> &t, const char *name, Args &&... args)
        {
            STREAM_PROPAGATE_ERROR(in.format().beginCompound(in, name));
            STREAM_PROPAGATE_ERROR(Serialize::read<U>(in, t.first, nullptr, args...));
            STREAM_PROPAGATE_ERROR(Serialize::read<V>(in, t.second, nullptr, args...));
            return in.format().endCompound(in, name);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const std::pair<U, V> &t, const char *name, Args &&... args)
        {
            out.format().beginCompound(out, name);
            Serialize::write<U>(out, t.first, "First", args...);
            Serialize::write<V>(out, t.second, "Second", args...);
            out.format().endCompound(out, name);
        }
    };

    template <typename T, typename... Configs, typename Hierarchy>
    StreamResult read(SerializeInStream &in, T &t, const char *name, const Hierarchy &hierarchy, StateTransmissionFlags flags)
    {
        return Operations<T, Configs...>::read(in, t, name, hierarchy, flags);
    }

    template <typename T, typename... Configs, typename Hierarchy>
    void write(SerializeOutStream &out, const T &t, const char *name, const Hierarchy &hierarchy, StateTransmissionFlags flags)
    {
        Operations<T, Configs...>::write(out, t, name, hierarchy, flags);
    }

    template <typename T>
    inline SerializeInStream &SerializeInStream::operator>>(T &t)
    {
        StreamResult result = read(*this, t, nullptr);
        if (result.mState != StreamState::OK) {
            setState(state() | std::ios_base::failbit);
            throw result;
        }
        return *this;
    }

    template <typename T>
    inline SerializeOutStream &SerializeOutStream::operator<<(const T &t)
    {
        write(*this, t, nullptr);
        return *this;
    }

    inline SerializeOutStream &SerializeOutStream::operator<<(const char *s)
    {
        write(*this, std::string_view { s }, nullptr);
        return *this;
    }

}
}