#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SafeBinaryFormatter : Formatter {
        SafeBinaryFormatter();

        virtual void beginPrimitive(SerializeOutStream &, const char *name, size_t typeId) override;
        virtual void beginPrimitive(SerializeInStream &, const char *name, size_t typeId) override;
    };

}
}