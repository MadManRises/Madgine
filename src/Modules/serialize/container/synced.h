#pragma once

#include "../../generic/noopfunctor.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "../../generic/offsetptr.h"
#include "unithelper.h"

namespace Engine {
namespace Serialize {

#define SYNCED(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::Synced<__VA_ARGS__>) 

    template <class T, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    class Synced : public Syncable<OffsetPtr>, public Serializable<OffsetPtr>, public UnitHelper<T>, private Observer {
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

        void setActive(bool active, bool existenceChanged)
        {
            if (!active) {
                if (mData != T {})
                    Observer::operator()(T {}, mData);
            }
            this->setItemActive(mData, active, existenceChanged);
            if (active) {
                if (mData != T {})
                    Observer::operator()(mData, T {});
            }
        }

    protected:
        void notify(const T &old)
        {
            if (this->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    out->write(mData, nullptr);
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
