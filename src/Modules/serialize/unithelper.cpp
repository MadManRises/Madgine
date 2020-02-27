#include "../moduleslib.h"

#include "unithelper.h"

#include "serializable.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"

namespace Engine {
namespace Serialize {    

	SerializableUnitBase *Engine::Serialize::convertPtr(SerializeInStream &in, size_t id)
    {
        return in.convertPtr(id);
    }

    bool SerializeUnitHelper::filter(SerializeOutStream &out, const SerializableUnitBase &item)
    {
        return /*item.filter(&out)*/ true; //TODO
    }

    bool SerializeUnitHelper::filter(SerializeOutStream &out, const SerializableBase &item)
    {
        return true;
    }

    void SerializeUnitHelper::setParent(SerializableUnitBase &item, SerializableUnitBase *parent)
    {
        item.setParent(parent);
    }

    void SerializeUnitHelper::setParent(SerializableBase &item, SerializableUnitBase *parent)
    {
    }

    void SerializeUnitHelper::beginExtendedItem(SerializeOutStream &out, const SerializableUnitBase &item)
    {
        out.format().beginExtendedCompound(out, "Item");
    }

    void SerializeUnitHelper::beginExtendedItem(SerializeInStream &in, const SerializableUnitBase &item)
    {
        in.format().beginExtendedCompound(in, "Item");
    }

    void SerializeUnitHelper::beginExtendedItem(SerializeOutStream &out, const SerializableBase &item)
    {
    }

    void SerializeUnitHelper::beginExtendedItem(SerializeInStream &in, const SerializableBase &item)
    {
    }

}
}
