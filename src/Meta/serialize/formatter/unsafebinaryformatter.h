#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT UnsafeBinaryFormatter : Formatter {
        UnsafeBinaryFormatter();

        virtual void beginContainerWrite(const char *name, uint32_t size) override;
        virtual StreamResult beginContainerRead(const char *name, bool sized) override;
        virtual StreamResult endContainerRead(const char *name) override;
        virtual bool hasContainerItem() override;

    private:
        std::stack<uint32_t> mContainerSizes;
    };

}
}