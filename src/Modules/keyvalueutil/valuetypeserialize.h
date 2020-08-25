#pragma once

#include "../serialize/streams/operations.h"

namespace Engine {
namespace Serialize {

    template <>
    struct MODULES_EXPORT Operations<ValueType> {
        static void read(SerializeInStream &in, ValueType &v, const char *name);
        static void write(SerializeOutStream &out, const ValueType &v, const char *name);

        template <typename... Args>
        static void read(SerializeInStream &in, ValueType &v, const char *name, Args &&... args)
        {
            read(in, v, name);
        }
        template <typename... Args>
        static void write(SerializeOutStream &out, const ValueType &v, const char *name, Args &&... args)
        {
            write(out, v, name);
        }
    };   

    
}
}