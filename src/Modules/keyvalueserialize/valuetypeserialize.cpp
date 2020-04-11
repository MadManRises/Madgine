#include "../moduleslib.h"

#include "valuetypeserialize.h"

#include "../serialize/streams/serializestream.h"

#include "../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {

    void read(SerializeInStream &in, ValueType &v, const char *name)
    {
        in.format().beginExtended(in, name);
        ValueType::Type type;
        in.read(type, "type");
        v.setType(type);
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

    void write(SerializeOutStream &out, const ValueType &v, const char *name)
    {
        out.format().beginExtended(out, name);
        out.write(v.type(), "type");
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
