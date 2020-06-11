#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SafeBinaryFormatter : Formatter {
        SafeBinaryFormatter();

        virtual void beginPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) override;
        virtual void beginPrimitive(SerializeInStream &, const char *name, uint8_t typeId) override;
    };

}
}