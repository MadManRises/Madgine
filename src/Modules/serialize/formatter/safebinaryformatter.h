#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SafeBinaryFormatter : Formatter {
        SafeBinaryFormatter();

        virtual void beginPrimitive(SerializeOutStream &, size_t typeId) override;
        virtual void beginPrimitive(SerializeInStream &, size_t typeId) override;

		virtual void writeEOL(SerializeOutStream &) override;
        virtual bool readEOL(SerializeInStream &) override;
    };

}
}