#pragma once

#include "../../generic/noopfunctor.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "offset.h"
#include "unithelper.h"

namespace Engine {
namespace Serialize {

#define SYNCED(Type, Name)                                                                          \
    ::Engine::Serialize::Synced<::Engine::Serialize::CombinedOffsetPtr<Self, __LINE__>, Type> Name; \
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
            this->read_state(in, this->mData);
            notify(old);
        }

        void readAction(SerializeInStream &in)
        {
            T old = mData;
            this->read_state(in, this->mData);
            notify(old);
        }

        void readState(SerializeInStream &in)
        {
            T old = mData;
            this->read_state(in, mData);
            if (!in.isMaster())
                this->read_id(in, mData);
            notify(old);
        }

        void writeState(SerializeOutStream &out) const
        {
            this->write_id(out, mData);
            this->write_state(out, mData);
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
                    Observer::operator()(T {}, mData);
            }
            this->setItemActive(mData, active);
            if (active) {
                Serializable<PtrOffset>::setActive(active);
                if (mData != T {})
                    Observer::operator()(mData, T {});
            }
        }

    protected:
        void notify(const T &old)
        {
            if (!PtrOffset::parent(this) || PtrOffset::parent(this)->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    this->write_state(*out, mData, nullptr);
                    out->endMessage();
                }
            }
            if (this->isActive()) {
                Observer::operator()(mData, old);
            }
        }

    private:
        T mData;
    };
}
}
