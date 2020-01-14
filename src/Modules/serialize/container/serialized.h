#pragma once

#include "../serializable.h"
#include "unithelper.h"

namespace Engine {
namespace Serialize {
    //TODO: Implement OffsetPtr interface
    template <typename T>
    struct Serialized : private UnitHelper<T>, SerializableBase {
        template <typename... _Ty>
        Serialized(_Ty &&... args)
            : mData(std::forward<_Ty>(args)...)
        {
            //this->setParent(mData, unit());
        }

        template <typename Ty>
        void operator=(Ty &&v)
        {
            mData = std::forward<Ty>(v);
        }

        T &operator*()
        {
            return mData;
        }

        const T &operator*() const
        {
            return mData;
        }

        T *operator->()
        {
            return &mData;
        }

        const T *operator->() const
        {
            return &mData;
        }

        T *ptr()
        {
            return &mData;
        }

        const T *ptr() const
        {
            return &mData;
        }

        operator const T &() const
        {
            return mData;
        }

        void readState(SerializeInStream &in)
        {
            this->read_state(in, mData);
        }

        void writeState(SerializeOutStream &out) const
        {
            this->write_state(out, mData);
        }

        void setDataSynced(bool b)
        {
            this->setItemDataSynced(mData, b);
        }

        void setActive(bool active, bool existenceChanged)
        {
            this->setItemActive(mData, active, existenceChanged);
        }

    private:
        T mData;
    };

}
}
