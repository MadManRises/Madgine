#include "interfaceslib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "streams/bufferedstream.h"

namespace Engine {
	namespace Serialize {

		Serializable::Serializable() :
			mUnit(SerializableUnitBase::findParent(this))
		{
			if (mUnit)
				mUnit->addSerializable(this);
		}

		Serializable::Serializable(const Serializable &) :
			mUnit(SerializableUnitBase::findParent(this))
		{
			if (mUnit)
				mUnit->addSerializable(this);
		}

		Serializable::Serializable(Serializable &&) :
			mUnit(SerializableUnitBase::findParent(this))
		{
			if (mUnit)
				mUnit->addSerializable(this);
		}

		void Serializable::applySerializableMap(const std::map<size_t, SerializableUnitBase*>& map)
		{
		}

		void Serializable::writeCreationData(SerializeOutStream & out) const
		{
		}

		void Serializable::setActive(bool b)
		{
		}

		std::set<BufferedOutStream*, CompareStreamId> Serializable::getMasterStateMessageTargets()
		{
			std::set<BufferedOutStream*, CompareStreamId> result;
			if (mUnit) {
				result = mUnit->getMasterMessageTargets();

				for (BufferedOutStream *out : result) {
					out->beginMessage(mUnit, STATE);
				}
			}

			return result;
		}

		void Serializable::sendState()
		{
			for (BufferedOutStream *out : getMasterStateMessageTargets()) {
				writeState(*out);
				out->endMessage();
			}
		}

		const TopLevelSerializableUnitBase * Serializable::topLevel() const
		{
			return mUnit ? mUnit->topLevel() : nullptr;
		}

		SerializableUnitBase * Serializable::unit() const
		{
			return mUnit;
		}

	}
}
