#pragma once

#include "../serialize/streams/serializestream.h"

namespace Engine {
namespace Serialize {

    template <>
    MODULES_EXPORT void read<ValueType>(SerializeInStream &in, ValueType &v, const char *name);
    template <>
    MODULES_EXPORT void write<ValueType>(SerializeOutStream &out, const ValueType &v, const char *name);

}
}