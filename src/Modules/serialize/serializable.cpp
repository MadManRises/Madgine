#include "../moduleslib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "streams/bufferedstream.h"

namespace Engine
{
	namespace Serialize
	{
		Serializable::Serializable()
		{                   
		}

		Serializable::Serializable(const Serializable&) :
			Serializable()
		{
		}

		Serializable::Serializable(Serializable&&) noexcept :
			Serializable()
		{
		}

		Serializable::~Serializable()
        {
		}

		Serializable& Serializable::operator=(const Serializable& other)
		{
			return *this;
		}

		void Serializable::applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map)
		{
		}

		void Serializable::setDataSynced(bool b)
		{
		}

		void Serializable::setActive(bool active)
		{
			assert(mActive != active);
			mActive = active;
		}

		bool Serializable::isActive() const
		{
			return mActive;
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
