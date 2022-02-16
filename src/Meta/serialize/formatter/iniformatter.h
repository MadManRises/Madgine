#pragma once

#include "Meta/serialize/formatter.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT IniFormatter : Formatter {

        IniFormatter();

        virtual void setupStream(std::iostream &) override;

        virtual void beginPrimitiveWrite(const char *name, uint8_t typeId) override;
        virtual void endPrimitiveWrite(const char *name, uint8_t typeId) override;

        virtual StreamResult beginPrimitiveRead(const char *name, uint8_t typeId) override;
        virtual StreamResult endPrimitiveRead(const char *name, uint8_t typeId) override;

        virtual StreamResult lookupFieldName(std::string &name) override;

        virtual void beginContainerWrite(const char *name, uint32_t size) override;
        virtual StreamResult beginContainerRead(const char *name, bool sized) override;
        virtual StreamResult endContainerRead(const char *name) override;
        virtual bool hasContainerItem() override;

    private:
        std::stack<uint32_t> mContainerSizes;
    };

}
}