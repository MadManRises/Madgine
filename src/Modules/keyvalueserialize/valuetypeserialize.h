#pragma once

namespace Engine {
namespace Serialize {

    MODULES_EXPORT void read(SerializeInStream &in, ValueType &v, const char *name = nullptr);
    MODULES_EXPORT void write(SerializeOutStream &out, const ValueType &v, const char *name = nullptr);

}
}