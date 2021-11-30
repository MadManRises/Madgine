#include "../metalib.h"

#include "valuetypeserialize.h"

#include "../serialize/streams/serializestream.h"

#include "../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {

    StreamResult Operations<ValueType>::read(SerializeInStream &in, ValueType &v, const char *name)
    {
        STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
        ValueTypeEnum type;
        STREAM_PROPAGATE_ERROR(Serialize::read(in, type, "type"));
        v.setType(ValueTypeDesc { type });
        return v.visit([&](auto &value) -> StreamResult {
            using T = std::remove_reference_t<decltype(value)>;
            if constexpr (PrimitiveType<T>) {
                STREAM_PROPAGATE_ERROR(in.format().beginPrimitive(in, name, PrimitiveTypeIndex_v<T>));
                STREAM_PROPAGATE_ERROR(in.readUnformatted(value));
                return in.format().endPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            } else
                throw 0;
        });
    }

    void Operations<ValueType>::write(SerializeOutStream &out, const ValueType &v, const char *name)
    {
        out.format().beginExtended(out, name, 1);
        Serialize::write(out, v.index().mIndex, "type");
        v.visit([&](const auto &value) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(value)>>;
            if constexpr (PrimitiveType<T>) {
                out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
                out.writeUnformatted(value);
                out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            } else
                throw 0;
        });
    }

}
}
