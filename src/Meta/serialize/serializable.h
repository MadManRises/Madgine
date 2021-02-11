#pragma once

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {

    struct SerializableBase {
        
    };

    template <typename OffsetPtr>
    struct Serializable : SerializableBase {
        bool isSynced() const
        {
            return OffsetPtr::parent(this) && OffsetPtr::parent(this)->mSynced;
        }

        bool isActive() const
        {
            return !OffsetPtr::parent(this) || SerializableUnitConstPtr { OffsetPtr::parent(this) }.isActive(OffsetPtr::template offset<SerializableDataUnit>());            
        }
    };
}
}
