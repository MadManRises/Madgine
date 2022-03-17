#pragma once

#include "Generic/container/containerevent.h"
#include "configs/configselector.h"
#include "configs/creator.h"
#include "configs/requestpolicy.h"
#include "configs/guard.h"

#include "hierarchy/serializableunitptr.h"

#include "streams/formattedbufferedstream.h"

#include "Generic/container/atomiccontaineroperation.h"

#include "container/physical.h"

#include "streams/pendingrequest.h"

#include "hierarchy/statetransmissionflags.h"

#include "streams/serializablemapholder.h"

namespace Engine {
namespace Serialize {

    META_EXPORT StreamResult convertSyncablePtr(FormattedSerializeStream &in, UnitId id, SyncableUnitBase *&out);
    META_EXPORT StreamResult convertSerializablePtr(FormattedSerializeStream &in, uint32_t id, SerializableDataUnit *&out);

    template <typename T, typename... Configs>
    struct Operations {
        static StreamResult read(FormattedSerializeStream &in, T &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {}, StateTransmissionFlags flags = 0)
        {
            if constexpr (PrimitiveType<T>) {
                return in.readPrimitive(t, name);
                //mLog.log(t);
            } else if constexpr (std::is_const_v<T>) {
                //Don't do anything here
                return {};
            } else if constexpr (std::derived_from<T, SyncableUnitBase>) {
                return t.readState(in, name, hierarchy, flags);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                return SerializableDataPtr { &t }.readState(in, name, hierarchy, flags);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                return Operations<decltype(TupleUnpacker::toTuple(std::declval<T &>())), Configs...>::read(in, TupleUnpacker::toTuple(t), name, hierarchy);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        static void write(FormattedSerializeStream &out, const T &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {}, StateTransmissionFlags flags = 0)
        {
            if constexpr (PrimitiveType<T>) {
                out.writePrimitive(t, name);
                //mLog.log(t);
            } else if constexpr (std::is_const_v<T>) {
                //Don't do anything here
            } else if constexpr (std::derived_from<T, SyncableUnitBase>) {
                t.writeState(out, name, hierarchy, flags);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                SerializableDataConstPtr { &t }.writeState(out, name, CallerHierarchyPtr { hierarchy }, flags);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                Operations<decltype(TupleUnpacker::toTuple(std::declval<T &>())), Configs...>::write(out, TupleUnpacker::toTuple(t), name, hierarchy);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        static StreamResult applyMap(FormattedSerializeStream &in, T &item, bool success)
        {
            if constexpr (Pointer<T>) {
                if (success) {
                    static_assert(std::derived_from<std::remove_pointer_t<T>, SerializableDataUnit>);
                    uint32_t ptr = reinterpret_cast<uintptr_t>(item);
                    if (ptr & 0x3) {
                        switch (static_cast<UnitIdTag>(ptr & 0x3)) {
                        case UnitIdTag::SYNCABLE:
                            if constexpr (std::derived_from<std::remove_pointer_t<T>, SyncableUnitBase>) {
                                UnitId id = (ptr >> 2);
                                SyncableUnitBase *unit;
                                STREAM_PROPAGATE_ERROR(convertSyncablePtr(in, id, unit));
                                item = static_cast<T>(unit);
                            } else {
                                throw 0;
                            }
                            break;
                        case UnitIdTag::SERIALIZABLE:
                            if constexpr (!std::derived_from<std::remove_pointer_t<T>, SyncableUnitBase>) {
                                uint32_t id = (ptr >> 2);
                                SerializableDataUnit *unit;
                                STREAM_PROPAGATE_ERROR(convertSerializablePtr(in, id, unit));
                                item = static_cast<T>(unit);
                            } else {
                                throw 0;
                            }
                            break;
                        default:
                            throw 0;
                        }
                    }
                } else {
                    item = nullptr;
                }
                return {};
            } else if constexpr (std::derived_from<T, SyncableUnitBase>) {
                return item.applyMap(in, success);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                return SerializableDataPtr { &item }.applyMap(in, success);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                return TupleUnpacker::accumulate(
                    TupleUnpacker::toTuple(item), [&](auto &t, StreamResult r) {
                        STREAM_PROPAGATE_ERROR(std::move(r));
                        return Operations<std::remove_reference_t<decltype(t)>>::applyMap(in, t, success);
                    },
                    StreamResult {});
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
                return {};
            }
        }

        static void setSynced(T &item, bool b)
        {
            if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setSynced(b);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    Serialize::setSynced(t, b);
                });
            }
        }

        static void setActive(T &item, bool active, bool existenceChanged)
        {
            if constexpr (std::derived_from<T, SyncableUnitBase>) {
                item.setActive(active, existenceChanged);
            } else if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setActive(active, existenceChanged);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                SerializableDataPtr { &item }.setActive(active, existenceChanged);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    Serialize::setActive(t, active, existenceChanged);
                });
            }
        }

        static void setParent(T &item, SerializableUnitBase *parent)
        {
            if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setParent(parent);
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    Serialize::setParent(t, parent);
                });
            }
        }
    };

    template <typename T, typename... Configs>
    struct Operations<std::unique_ptr<T>, Configs...> {

        static StreamResult read(FormattedSerializeStream &in, const std::unique_ptr<T> &p, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            return Operations<T, Configs...>::read(in, *p, name, hierarchy);
        }

        static void write(FormattedSerializeStream &out, const std::unique_ptr<T> &p, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            Operations<T, Configs...>::write(out, *p, name, hierarchy);
        }

        static StreamResult applyMap(FormattedSerializeStream &in, const std::unique_ptr<T> &p, bool success)
        {
            return Operations<T, Configs...>::applyMap(in, *p, success);
        }

        static void setSynced(const std::unique_ptr<T> &p, bool b)
        {
            Operations<T, Configs...>::setSynced(*p, b);
        }

        static void setActive(const std::unique_ptr<T> &p, bool active, bool existenceChanged)
        {
            Operations<T, Configs...>::setActive(*p, active, existenceChanged);
        }

        static void setParent(const std::unique_ptr<T> &p, SerializableUnitBase *parent)
        {
            Operations<T, Configs...>::setParent(*p, parent);
        }
    };

    template <typename T, typename... Configs>
    struct Operations<const std::unique_ptr<T>, Configs...> : Operations<std::unique_ptr<T>, Configs...> {
    };

    template <typename... Ty, typename... Configs>
    struct Operations<std::tuple<Ty...>, Configs...> {

        static StreamResult read(FormattedSerializeStream &in, std::tuple<Ty...> &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            STREAM_PROPAGATE_ERROR(in.beginContainerRead(name, false));
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                t, [&](auto &e, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return Serialize::read(in, e, nullptr, hierarchy);
                },
                StreamResult {}));
            return in.endContainerRead(name);
        }

        static void write(FormattedSerializeStream &out, const std::tuple<Ty...> &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            out.beginContainerWrite(name);
            TupleUnpacker::forEach(t, [&](const auto &e) {
                Serialize::write(out, e, "Element", hierarchy);
            });
            out.endContainerWrite(name);
        }
    };

    template <typename... Ty, typename... Configs>
    struct Operations<std::tuple<Ty &...>, Configs...> {

        static StreamResult read(FormattedSerializeStream &in, std::tuple<Ty &...> t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            STREAM_PROPAGATE_ERROR(in.beginContainerRead(name, false));
            STREAM_PROPAGATE_ERROR(TupleUnpacker::accumulate(
                t, [&](auto &e, StreamResult r) {
                    STREAM_PROPAGATE_ERROR(std::move(r));
                    return Serialize::read(in, e, nullptr, hierarchy);
                },
                StreamResult {}));
            return in.endContainerRead(name);
        }

        static void write(FormattedSerializeStream &out, const std::tuple<const Ty &...> t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            out.beginContainerWrite(name);
            TupleUnpacker::forEach(t, [&](const auto &e) {
                Serialize::write(out, e, "Element", hierarchy);
            });
            out.endContainerWrite(name);
        }
    };

    template <typename U, typename V, typename... Configs>
    struct Operations<std::pair<U, V>, Configs...> {

        static StreamResult read(FormattedSerializeStream &in, std::pair<U, V> &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            STREAM_PROPAGATE_ERROR(in.beginCompoundRead(name));
            STREAM_PROPAGATE_ERROR(Serialize::read<U>(in, t.first, nullptr, hierarchy));
            STREAM_PROPAGATE_ERROR(Serialize::read<V>(in, t.second, nullptr, hierarchy));
            return in.endCompoundRead(name);
        }

        static void write(FormattedSerializeStream &out, const std::pair<U, V> &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {})
        {
            out.beginCompoundWrite(name);
            Serialize::write<U>(out, t.first, "First", hierarchy);
            Serialize::write<V>(out, t.second, "Second", hierarchy);
            out.endCompoundWrite(name);
        }
    };

    template <typename T, typename... Configs>
    StreamResult read(FormattedSerializeStream &in, T &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {}, StateTransmissionFlags flags = 0)
    {
        SerializableListHolder holder { in };

        if (flags & StateTransmissionFlags_Activation)
            setActive(t, false, false);

        StreamResult result = TupleUnpacker::invoke(Operations<T, Configs...>::read, in, t, name, hierarchy, flags);

        if (flags & StateTransmissionFlags_ApplyMap) {
            assert(in.manager());
            STREAM_PROPAGATE_ERROR(applyMap(in, t, result.mState == StreamState::OK));
        }

        if (flags & StateTransmissionFlags_Activation)
            setActive(t, true, false);

        return result;
    }

    template <typename T, typename... Configs>
    void write(FormattedSerializeStream &out, const T &t, const char *name, const CallerHierarchyBasePtr &hierarchy = {}, StateTransmissionFlags flags = 0)
    {
        SerializableMapHolder holder { out };

        TupleUnpacker::invoke(Operations<T, Configs...>::write, out, t, name, hierarchy, flags);
    }

    template <typename T, typename... Configs>
    StreamResult applyMap(FormattedSerializeStream &in, T &t, bool success)
    {
        if constexpr (requires { &Operations<T>::applyMap; })
            return Operations<T, Configs...>::applyMap(in, t, success);
        else
            return {};
    }

    template <typename T, typename... Configs>
    void setSynced(T &t, bool b)
    {
        if constexpr (requires { &Operations<T>::setSynced; })
            Operations<T, Configs...>::setSynced(t, b);
    }

    template <typename T, typename... Configs>
    void setActive(T &t, bool active, bool existenceChanged)
    {
        if constexpr (requires { &Operations<T>::setActive; })
            Operations<T, Configs...>::setActive(t, active, existenceChanged);
    }

    template <typename T>
    void setParent(T &t, SerializableDataUnit *parent)
    {
    }

    template <typename T, typename... Configs>
    void setParent(T &t, SerializableUnitBase *parent)
    {
        if constexpr (requires { &Operations<T, Configs...>::setParent; })
            Operations<T, Configs...>::setParent(t, parent);
    }

    template <typename T, typename... Configs>
    StreamResult readAction(T &t, FormattedBufferedStream &in, PendingRequest *request, const CallerHierarchyBasePtr &hierarchy = {})
    {
        auto guard = GuardSelector<Configs...>::guard(hierarchy);
        return Operations<T, Configs...>::readAction(t, in, request, hierarchy);
    }

    template <typename T, typename... Configs>
    StreamResult readRequest(T &t, FormattedBufferedStream &inout, TransactionId id, const CallerHierarchyBasePtr &hierarchy = {})
    {
        return Operations<T, Configs...>::readRequest(t, inout, id, hierarchy);
    }

    template <typename T, typename... Configs>
    void writeAction(const T &t, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
    {
        Operations<T, Configs...>::writeAction(t, outStreams, data, hierarchy);
    }

    template <typename T, typename... Configs>
    void writeRequest(const T &t, FormattedBufferedStream &out, const void *data, const CallerHierarchyBasePtr &hierarchy = {})
    {
        Operations<T, Configs...>::writeRequest(t, out, data, hierarchy);
    }

}
}