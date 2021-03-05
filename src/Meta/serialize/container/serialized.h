#pragma once

#include "../serializable.h"
#include "../unithelper.h"

namespace Engine {
namespace Serialize {
    //TODO: Implement OffsetPtr interface
    template <typename T>
    struct Serialized : SerializableBase {
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

        template <typename Ty>
        void operator+=(Ty &&v)
        {
            mData += std::forward<Ty>(v);
        }

        template <typename Ty>
        void operator-=(Ty &&v)
        {
            mData -= std::forward<Ty>(v);
        }

        StreamResult readState(SerializeInStream &in, const char *name = nullptr)
        {
            return read(in, mData, name);
        }

        void writeState(SerializeOutStream &out, const char *name = nullptr) const
        {
            write(out, mData, name);
        }

        void setDataSynced(bool b)
        {
            UnitHelper<T>::setItemDataSynced(mData, b);
        }

        void setActive(bool active, bool existenceChanged)
        {
            UnitHelper<T>::setItemActive(mData, active, existenceChanged);
        }

        void applySerializableMap(SerializeInStream &in)
        {
            UnitHelper<T>::applyMap(in, mData);
        }

        void setParent(SerializableUnitBase *parent)
        {
            UnitHelper<T>::setItemParent(mData, parent);
        }

    private:
        T mData;
    };

}
}
