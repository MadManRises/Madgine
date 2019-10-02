#pragma once

#include "../../generic/noopfunctor.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "offset.h"
#include "unithelper.h"
#include "../../generic/tupleunpacker.h"

namespace Engine {
namespace Serialize {

#define SYNCED(Name, ...)                                                                          \
    ::Engine::Serialize::Synced<::Engine::Serialize::CombinedOffsetPtr<Self, __LINE__>, __VA_ARGS__> Name; \
    DEFINE_COMBINED_OFFSET(Name)

    template <typename PtrOffset, class T, typename Observer = NoOpFunctor>
    class Synced : public Syncable<PtrOffset>, public Serializable<PtrOffset>, public UnitHelper<T>, private Observer {
    public:
        template <class... _Ty>
        Synced(_Ty &&... args)
            : mData(std::forward<_Ty>(args)...)
        {
        }

        Observer &observer()
        {
            return *this;
        }

        template <class Ty>
        void operator=(Ty &&v)
        {
            if (mData != v) {
                T old = mData;
                mData = std::forward<Ty>(v);
                notify(old);
            }
        }

        template <class Ty>
        void operator+=(Ty &&v)
        {
            T old = mData;
            mData += std::forward<Ty>(v);
            notify(old);
        }

        template <class Ty>
        void operator-=(Ty &&v)
        {
            T old = mData;
            mData -= std::forward<Ty>(v);
            notify(old);
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

        void readRequest(BufferedInOutStream &in)
        {
            T old = mData;
            in.read(mData);            
            notify(old);
        }

        void readAction(SerializeInStream &in)
        {
            T old = mData;
            in.read(mData);
            notify(old);
        }

        void readState(SerializeInStream &in, const char *name)
        {
            T old = mData;
            in.read(mData, name);            
            notify(old);
        }

        void writeState(SerializeOutStream &out, const char *name) const
        {
            out.write(mData, name);
        }

        void setDataSynced(bool b)
        {
            this->setItemDataSynced(mData, b);
        }

        void setActive(bool active)
        {
            if (!active) {
                Serializable<PtrOffset>::setActive(active);
                if (mData != T {})
                    TupleUnpacker::invoke(&Observer::operator(), static_cast<Observer *>(this), T {}, mData);
            }
            this->setItemActive(mData, active);
            if (active) {
                Serializable<PtrOffset>::setActive(active);
                if (mData != T {})
                    TupleUnpacker::invoke(&Observer::operator(), static_cast<Observer *>(this), mData, T {});
            }
        }

    protected:
        void notify(const T &old)
        {
            if (!PtrOffset::parent(this) || PtrOffset::parent(this)->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    out->write(mData, nullptr);
                    out->endMessage();
                }
            }
            if (this->isActive()) {
                TupleUnpacker::invoke(&Observer::operator(), static_cast<Observer *>(this), mData, old);
            }
        }

    private:
        T mData;
    };
}
}
