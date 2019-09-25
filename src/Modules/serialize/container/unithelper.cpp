#include "../../moduleslib.h"

#include "unithelper.h"

#include "../toplevelserializableunit.h"
#include "../serializable.h"

namespace Engine
{
	namespace Serialize
	{
//		template struct UnitHelper<ValueType, false>;

		bool SerializeUnitHelper::filter(SerializeOutStream& out, const SerializableUnitBase& item)
		{
			return /*item.filter(&out)*/true; //TODO
		}

		bool SerializeUnitHelper::filter(SerializeOutStream &out, const SerializableBase &item)
		{
			return true;
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase *> &map, SerializableUnitBase &item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::applyMap(const std::map<size_t, SerializableUnitBase *> &map, SerializableBase &item)
		{
			item.applySerializableMap(map);
		}

		void SerializeUnitHelper::setParent(SerializableUnitBase& item, SerializableUnitBase* parent)
		{
			item.setParent(parent);
		}

		void SerializeUnitHelper::setParent(SerializableBase &item, SerializableUnitBase *parent)
		{
		}

	}
}
