#pragma once

#include "../serialize/streams/operations.h"

namespace Engine {
namespace Serialize {

    template <>
    struct META_EXPORT Operations<ValueType> {
        static StreamResult read(FormattedSerializeStream &in, ValueType &v, const char *name);
        static void write(FormattedSerializeStream &out, const ValueType &v, const char *name);

        template <typename... Args>
        static StreamResult read(FormattedSerializeStream &in, ValueType &v, const char *name, Args &&...args)
        {
            return read(in, v, name);
        }
        template <typename... Args>
        static void write(FormattedSerializeStream &out, const ValueType &v, const char *name, Args &&...args)
        {
            write(out, v, name);
        }
    };   

    
}
}