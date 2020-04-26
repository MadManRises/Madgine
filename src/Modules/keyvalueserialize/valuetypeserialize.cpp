#include "../moduleslib.h"

#include "valuetypeserialize.h"

#include "../serialize/streams/serializestream.h"

#include "../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {

    template <>
    void read<ValueType>(SerializeInStream &in, ValueType &v, const char *name)
    {
        in.format().beginExtended(in, name);
        ValueTypeEnum type;
        in.read(type, "type");
        v.setType(ValueTypeDesc { type });
        v.visit([&](auto &value) {
            using T = std::remove_reference_t<decltype(value)>;
            if constexpr (isPrimitiveType_v<T> && !std::is_same_v<T, std::string_view>) {
                in.format().beginPrimitive(in, name, PrimitiveTypeIndex_v<T>, true);
                in.readUnformatted(value);
                in.format().endPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            } else
                throw 0;
        });
    }

    template <>
    void write<ValueType>(SerializeOutStream &out, const ValueType &v, const char *name)
    {
        out.format().beginExtended(out, name);
        out.write(v.index().mIndex, "type");
        v.visit([&](const auto &value) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(value)>>;
            if constexpr (isPrimitiveType_v<T>) {
                out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>, true);
                out.writeUnformatted(value);
                out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            } else
                throw 0;
        });
    }

}
}
