#include "../moduleslib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "streams/bufferedstream.h"

namespace Engine
{
	namespace Serialize
	{
		Serializable::Serializable(bool local) :
			mUnit(local ? nullptr : SerializableUnitBase::findParent(this))
		{
			if (mUnit)
			{
				mUnit->addSerializable(this);
				assert(!mUnit->isSynced());
			}
			else
			{
				mActive = true;
			}
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
			if (mUnit)
				assert(!mUnit->isSynced());
		}

		Serializable& Serializable::operator=(const Serializable& other)
		{
			return *this;
		}

		void Serializable::applySerializableMap(const std::map<size_t, SerializableUnitBase*>& map)
		{
		}

		void Serializable::writeCreationData(SerializeOutStream& out) const
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

		bool Serializable::isSynced() const
		{
			return mUnit ? mUnit->isSynced() : true;
		}

		std::set<BufferedOutStream*, CompareStreamId> Serializable::getMasterStateMessageTargets() const
		{
			std::set<BufferedOutStream*, CompareStreamId> result;
			if (mUnit)
			{
				result = mUnit->getMasterMessageTargets();

				for (BufferedOutStream* out : result)
				{
					out->beginMessage(mUnit, STATE);
				}
			}

			return result;
		}

		void Serializable::sendState()
		{
			for (BufferedOutStream* out : getMasterStateMessageTargets())
			{
				writeState(*out);
				out->endMessage();
			}
		}

		const TopLevelSerializableUnitBase* Serializable::topLevel() const
		{
			return mUnit ? mUnit->topLevel() : nullptr;
		}

		SerializableUnitBase* Serializable::unit() const
		{
			return mUnit;
		}
	}
}
