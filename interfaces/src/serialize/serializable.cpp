#include "interfaceslib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "streams/bufferedstream.h"

namespace Engine
{
	namespace Serialize
	{
		Serializable::Serializable(bool local) :
			mUnit(local ? nullptr : SerializableUnitBase::findParent(this)),
			mLocallyActive(false)
		{
			if (mUnit)
			{
				mUnit->addSerializable(this);
				assert(!mUnit->isActive());
			}
			else
			{
				mLocallyActive = true;
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
				assert(!mUnit->isActive());
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

		void Serializable::setActiveFlag(bool b)
		{
		}

		void Serializable::notifySetActive(bool active)
		{
			assert(mLocallyActive != active);
			mLocallyActive = active;
		}

		bool Serializable::isLocallyActive() const
		{
			return mLocallyActive;
		}

		bool Serializable::isActive() const
		{
			return mUnit ? mUnit->isActive() : true;
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
