#pragma once

#include "../serialize/operations.h"

namespace Engine {
namespace Serialize {

    template <>
    struct META_EXPORT Operations<ValueType> {
        static StreamResult read(FormattedSerializeStream &in, ValueType &v, const char *name, const CallerHierarchyBasePtr &hierarchy = {});
        static void write(FormattedSerializeStream &out, const ValueType &v, const char *name, const CallerHierarchyBasePtr &hierarchy = {});
    };   

    
}
}