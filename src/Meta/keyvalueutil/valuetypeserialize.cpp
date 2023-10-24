#include "../metalib.h"

#include "valuetypeserialize.h"

#include "../serialize/streams/serializestream.h"

#include "../keyvalue/valuetype.h"

#include "../serialize/streams/formattedserializestream.h"

#include "Meta/serialize/operations.h"

namespace Engine {
namespace Serialize {

    StreamResult Operations<ValueType>::read(FormattedSerializeStream &in, ValueType &v, const char *name, const CallerHierarchyBasePtr &hierarchy)
    {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead(name, 1));
        ValueTypeEnum type;
        STREAM_PROPAGATE_ERROR(Serialize::read(in, type, "type"));
        v.setType(ValueTypeDesc { type });
        return v.visit([&](auto &value) -> StreamResult {
            using T = std::remove_reference_t<decltype(value)>;
            if constexpr (PrimitiveType<T>) {
                return Serialize::read(in, value, name);
            } else if constexpr (std::same_as<T, std::monostate>) {
                Void v;
                return Serialize::read(in, v, name);
            } else
                throw 0;
        });
    }

    void Operations<ValueType>::write(FormattedSerializeStream &out, const ValueType &v, const char *name, const CallerHierarchyBasePtr &hierarchy)    
    {
        out.beginExtendedWrite(name, 1);
        Serialize::write(out, v.index().mIndex, "type");
        v.visit([&](const auto &value) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(value)>>;
            if constexpr (PrimitiveType<T>) {
                Serialize::write(out, value, name);
            } else if constexpr (std::same_as<T, std::monostate>){
                Serialize::write(out, Void {}, name);
            } else
                throw 0;
        });
    }

    StreamResult Operations<ValueType>::scanStream(FormattedSerializeStream& in, const char* name, const Lambda<ScanCallback>& callback) {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead(name, 1));
        ValueTypeEnum type;
        STREAM_PROPAGATE_ERROR(Serialize::read(in, type, "type"));
        ValueType v;
        v.setType(ValueTypeDesc { type });
        return v.visit([&](auto &value) -> StreamResult {
            using T = std::remove_reference_t<decltype(value)>;
            if constexpr (PrimitiveType<T>) {
                return Serialize::scanStream<T>(in, name, callback);
            } else if constexpr (std::same_as<T, std::monostate>) {                
                return Serialize::scanStream<Void>(in, name, callback);
            } else
                throw 0;
        });
    }

}
}
