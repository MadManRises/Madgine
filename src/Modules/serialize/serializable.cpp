#include "../moduleslib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "streams/bufferedstream.h"

namespace Engine {
namespace Serialize {
    SerializableBase::SerializableBase()
    {
    }

    SerializableBase::SerializableBase(const SerializableBase &)
        : SerializableBase()
    {
    }

    SerializableBase::SerializableBase(SerializableBase &&) noexcept
        : SerializableBase()
    {
    }

    SerializableBase::~SerializableBase()
    {
    }

    SerializableBase &SerializableBase::operator=(const SerializableBase &other)
    {
        return *this;
    }

    void SerializableBase::applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map)
    {
    }

    void SerializableBase::setDataSynced(bool b)
    {
    }

    void SerializableBase::setActive(bool active)
    {
        assert(mActive != active);
        mActive = active;
    }

    bool SerializableBase::isActive() const
    {
        return mActive;
    }

    void SerializableBase::writeCreationData(SerializeOutStream &) const
    {
    }

    /*bool Serializable::isSynced() const
		{
			return mUnit ? mUnit->isSynced() : true;
		}*/

    /*const TopLevelSerializableUnitBase* Serializable::topLevel() const
		{
			return mUnit ? mUnit->topLevel() : nullptr;
		}

		SerializableUnitBase* Serializable::unit() const
		{
			return mUnit;
		}*/
}
}
