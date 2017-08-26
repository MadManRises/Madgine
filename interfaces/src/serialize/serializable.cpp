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

		void Serializable::activate()
		{
		}

		std::list<BufferedOutStream*> Serializable::getMasterStateMessageTargets()
		{
			return mUnit ? mUnit->getMasterMessageTargets(false) : std::list<BufferedOutStream*>();
		}

		void Serializable::sendState()
		{
			for (BufferedOutStream *out : getMasterStateMessageTargets()) {
				writeState(*out);
				out->endMessage();
			}
		}

		TopLevelSerializableUnitBase * Serializable::topLevel() const
		{
			return mUnit ? mUnit->topLevel() : nullptr;
		}

		SerializableUnitBase * Serializable::unit() const
		{
			return mUnit;
		}

	}
}
