#include "../metalib.h"

#include "valuetypeserialize.h"

#include "../serialize/streams/serializestream.h"

#include "../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {

    StreamResult Operations<ValueType>::read(FormattedSerializeStream &in, ValueType &v, const char *name)
    {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead(name, 1));
        ValueTypeEnum type;
        STREAM_PROPAGATE_ERROR(Serialize::read(in, type, "type"));
        v.setType(ValueTypeDesc { type });
        return v.visit([&](auto &value) -> StreamResult {
            using T = std::remove_reference_t<decltype(value)>;
            if constexpr (PrimitiveType<T>) {
                return Serialize::read(in, value, name);
            } else
                throw 0;
        });
    }

    void Operations<ValueType>::write(FormattedSerializeStream &out, const ValueType &v, const char *name)
    {
        out.beginExtendedWrite(name, 1);
        Serialize::write(out, v.index().mIndex, "type");
        v.visit([&](const auto &value) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(value)>>;
            if constexpr (PrimitiveType<T>) {
                Serialize::write(out, value, name);
            } else
                throw 0;
        });
    }

}
}
