#pragma once

#include "../../generic/copy_traits.h"
#include "../../generic/functor.h"
#include "../../generic/offsetptr.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "../unithelper.h"
#include "../streams/operations.h"

//TODO rewrite to modern operations
namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION2(PlusAssign, operator+=);
    DERIVE_FUNCTION2(MinusAssign, operator-=);

    namespace SyncedOperation {
        enum Value {
            SET,
            ADD,
            SUB
        };
    };

#define SYNCED(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::Synced<__VA_ARGS__>)

    template <typename T, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct Synced : Syncable<OffsetPtr>, Serializable<OffsetPtr>, CopyTraits<T>, private Observer {

        friend struct Operations<Synced<T, Observer, OffsetPtr>>;

        template <typename... _Ty>
        Synced(_Ty &&... args)
            : mData(std::forward<_Ty>(args)...)
        {
        }

        Observer &observer()
        {
            return *this;
        }

        template <typename Ty>
        void operator=(Ty &&v)
        {
            if (mData != v) {
                if (this->isMaster()) {
                    T old = mData;
                    mData = std::forward<Ty>(v);
                    notify(old);
                } else {
                    T data = std::forward<Ty>(v);
                    this->writeRequest(SyncedOperation::SET, &data);
                }
            }
        }

        template <typename Ty>
        void operator+=(Ty &&v)
        {
            if (this->isMaster()) {
                T old = mData;
                mData += std::forward<Ty>(v);
                notify(old);
            } else {
                T data = std::forward<Ty>(v);
                this->writeRequest(SyncedOperation::ADD, &data);
            }
        }

        template <typename Ty>
        void operator-=(Ty &&v)
        {
            if (this->isMaster()) {
                T old = mData;
                mData -= std::forward<Ty>(v);
                notify(old);
            } else {
                T data = std::forward<Ty>(v);
                this->writeRequest(SyncedOperation::SUB, &data);
            }
        }

        T *operator->()
        {
            return &mData;
        }

        T *ptr()
        {
            return &mData;
        }

        operator const T &() const
        {
            return mData;
        }

        void setDataSynced(bool b)
        {
            UnitHelper<T>::setItemDataSynced(mData, b);
        }

        void setActive(bool active, bool existenceChanged)
        {
            if (!active) {
                if (mData != T {})
                    Observer::operator()(T {}, mData);
            }
            UnitHelper<T>::setItemActive(mData, active, existenceChanged);
            if (active) {
                if (mData != T {})
                    Observer::operator()(mData, T {});
            }
        }

        void applySerializableMap(SerializeInStream &in)
        {
            UnitHelper<T>::applyMap(in, mData);
        }

        void setParent(SerializableUnitBase *parent)
        {
            UnitHelper<T>::setItemParent(mData, parent);
        }

    protected:
        void notify(const T &old, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            if (this->isSynced()) {
                this->writeAction(SyncedOperation::SET, &mData, answerTarget, answerId);
            }
            if (this->isActive()) {
                Observer::operator()(mData, old);
            }
        }

    private:
        T mData;
    };

    template <typename T, typename Observer, typename OffsetPtr>
    struct Operations<Synced<T, Observer, OffsetPtr>> {
        template <typename... Args>
        static void readRequest(Synced<T, Observer, OffsetPtr> &synced, BufferedInOutStream &inout, TransactionId id, Args &&... args)
        {
            if (synced.isMaster()) {
                SyncedOperation::Value op;
                Serialize::read(inout, op, nullptr);
                T old = synced.mData;
                T value;
                Serialize::read(inout, value, nullptr);
                switch (op) {
                case SyncedOperation::SET:
                    synced.mData = value;
                    break;
                case SyncedOperation::ADD:
                    if constexpr (has_function_PlusAssign_v<T>)
                        synced.mData += value;
                    else
                        throw 0;
                    break;
                case SyncedOperation::SUB:
                    if constexpr (has_function_MinusAssign_v<T>)
                        synced.mData -= value;
                    else
                        throw 0;
                    break;
                }
                synced.notify(old, inout.id(), id);
            } else {
                BufferedOutStream *out = synced.getSlaveActionMessageTarget(inout.id(), id);
                out->pipe(inout);
                out->endMessage();
            }
        }

                template <typename... Args>
        static void writeRequest(const Synced<T, Observer, OffsetPtr> &synced, int op, const void *data, BufferedOutStream *out, Args &&... args)
        {
            Serialize::write(*out, op, nullptr);
            Serialize::write(*out, *static_cast<const T *>(data), nullptr);
            out->endMessage();
        }

        template <typename... Args>
        static void readAction(Synced<T, Observer, OffsetPtr> &synced, SerializeInStream &in, PendingRequest *request, Args&&... args)
        {
            SyncedOperation::Value op;
            Serialize::read(in, op, nullptr);
            T old = synced.mData;
            T value;
            Serialize::read(in, value, nullptr);
            switch (op) {
            case SyncedOperation::SET:
                synced.mData = value;
                break;
            case SyncedOperation::ADD:
                if constexpr (has_function_PlusAssign_v<T>)
                    synced.mData += value;
                else
                    throw 0;
                break;
            case SyncedOperation::SUB:
                if constexpr (has_function_MinusAssign_v<T>)
                    synced.mData -= value;
                else
                    throw 0;
                break;
            }
            synced.notify(old);
        }

                template <typename... Args>
        static void writeAction(const Synced<T, Observer, OffsetPtr> &synced, int op, const void *data, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, Args &&... args)
        {
            for (BufferedOutStream *out : outStreams) {
                Serialize::write(*out, op, nullptr);
                Serialize::write(*out, *static_cast<const T *>(data), nullptr);
                out->endMessage();
            }
        }

        template <typename... Args>
        static void read(SerializeInStream &in, Synced<T, Observer, OffsetPtr> &synced, const char *name, Args &&... args)
        {
            T old = synced.mData;
            Serialize::read(in, synced.mData, name);
            synced.notify(old);
        }

        template <typename... Args>
        static void write(SerializeOutStream &out, const Synced<T, Observer, OffsetPtr> &synced, const char *name, Args &&... args)
        {
            Serialize::write(out, synced.mData, name);
        }
    };
}
}
