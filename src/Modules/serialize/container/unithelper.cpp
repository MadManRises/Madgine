#include "../../moduleslib.h"

#include "unithelper.h"

#include "../toplevelserializableunit.h"
#include "../serializable.h"

namespace Engine
{
	namespace Serialize
	{
//		template struct UnitHelper<ValueType, false>;


		void SerializeUnitHelper::read_id(SerializeInStream &in, SerializableUnitBase &item)
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

		bool SerializeUnitHelper::filter(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			return /*item.filter(&out)*/true; //TODO
		}

		bool SerializeUnitHelper::filter(SerializeOutStream& out, const Serializable& item)
		{
			return true;
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase *> &map, SerializableUnitBase &item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase *> &map, Serializable &item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::setParent(SerializableUnitBase& item, SerializableUnitBase* parent)
		{
			item.setParent(parent);
		}

		void SerializeUnitHelper::setParent(Serializable& item, SerializableUnitBase* parent)
		{
		}

		void SerializeUnitHelper::setItemDataSynced(SerializableUnitBase& item, bool b)
		{
			item.setDataSynced(b);
		}

		void SerializeUnitHelper::setItemDataSynced(Serializable& item, bool b)
		{
		}

		void SerializeUnitHelper::setItemActive(SerializableUnitBase& item, bool active)
		{
			item.setActive(active);
		}

		void SerializeUnitHelper::setItemActive(Serializable& item, bool active)
		{
		}

	}
}
