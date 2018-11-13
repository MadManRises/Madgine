#include "../../interfaceslib.h"

#include "unithelper.h"

#include "../toplevelserializableunit.h"
#include "../serializable.h"

namespace Engine
{
	namespace Serialize
	{
		template struct UnitHelper<ValueType, false>;


		void SerializeUnitHelper::read_state(SerializeInStream& in, Serializable& item)
		{
			item.readState(in);
		}

		void SerializeUnitHelper::read_state(SerializeInStream& in, SerializableUnitBase& item)
		{
			item.readState(in);
		}

		void SerializeUnitHelper::read_id(SerializeInStream& in, SerializableUnitBase& item)
		{
			item.readId(in);
		}

		void SerializeUnitHelper::read_id(SerializeInStream& in, Serializable& item)
		{
		}

		void SerializeUnitHelper::write_id(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			item.writeId(out);
		}

		void SerializeUnitHelper::write_id(SerializeOutStream& out, const Serializable& item)
		{
		}

		void SerializeUnitHelper::write_creation(SerializeOutStream& out, const Serializable& item)
		{
			item.writeCreationData(out);
		}

		void SerializeUnitHelper::write_creation(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			item.writeCreationData(out);
		}

		void SerializeUnitHelper::write_state(SerializeOutStream& out, const Serializable& item)
		{
			item.writeState(out);
		}

		void SerializeUnitHelper::write_state(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			item.writeState(out);
		}

		bool SerializeUnitHelper::filter(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			return item.filter(&out);
		}

		bool SerializeUnitHelper::filter(SerializeOutStream& out, const Serializable& item)
		{
			return true;
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase*>& map, SerializableUnitBase& item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase*>& map, Serializable& item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::postConstruct(SerializableUnitBase& item)
		{
			item.postConstruct();
		}

		void SerializeUnitHelper::postConstruct(Serializable& item)
		{
		}

		void SerializeUnitHelper::setParent(SerializableUnitBase& item, SerializableUnitBase* parent)
		{
			item.setParent(parent);
		}

		void SerializeUnitHelper::setParent(Serializable& item, SerializableUnitBase* parent)
		{
		}

		void SerializeUnitHelper::setItemActiveFlag(SerializableUnitBase& item, bool b)
		{
			item.setActiveFlag(b);
		}

		void SerializeUnitHelper::setItemActiveFlag(Serializable& item, bool b)
		{
		}

		void SerializeUnitHelper::notifySetItemActive(SerializableUnitBase& item, bool active)
		{
			item.notifySetActive(active);
		}

		void SerializeUnitHelper::notifySetItemActive(Serializable& item, bool active)
		{
		}

	}
}
