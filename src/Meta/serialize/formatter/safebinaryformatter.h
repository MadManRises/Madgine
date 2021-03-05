#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SafeBinaryFormatter : Formatter {
        SafeBinaryFormatter();

        virtual void beginPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) override;
        virtual StreamResult beginPrimitive(SerializeInStream &, const char *name, uint8_t typeId) override;

        virtual void beginContainer(SerializeOutStream &, const char *name, uint32_t size) override;
        virtual StreamResult beginContainer(SerializeInStream &, const char *name, bool sized) override;
        virtual StreamResult endContainer(SerializeInStream &, const char *name) override;
        virtual bool hasContainerItem(SerializeInStream &) override;

    private:
        std::stack<uint32_t> mContainerSizes;
    };

}
}