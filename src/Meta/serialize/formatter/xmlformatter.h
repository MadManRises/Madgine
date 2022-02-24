#pragma once

#include "../streams/formatter.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT XMLFormatter : Formatter {

        XMLFormatter();

        virtual void setupStream(std::iostream &) override;

        virtual void beginExtendedWrite(const char *name, size_t count) override;

        virtual void beginCompoundWrite(const char *name) override;
        virtual void endCompoundWrite(const char *name) override;

        virtual StreamResult beginExtendedRead(const char *name, size_t count) override;

        virtual StreamResult beginCompoundRead(const char *name) override;
        virtual StreamResult endCompoundRead(const char *name) override;

        virtual void beginPrimitiveWrite(const char *name, uint8_t typeId) override;
        virtual void endPrimitiveWrite(const char *name, uint8_t typeId) override;

        virtual StreamResult beginPrimitiveRead(const char *name, uint8_t typeId) override;
        virtual StreamResult endPrimitiveRead(const char *name, uint8_t typeId) override;

        virtual StreamResult lookupFieldName(std::string &name) override;

        virtual void beginContainerWrite(const char *name, uint32_t size) override;
        virtual void endContainerWrite(const char *name) override;
        virtual StreamResult beginContainerRead(const char *name, bool sized) override;
        virtual StreamResult endContainerRead(const char *name) override;
        virtual bool hasContainerItem() override;

        std::string indent();

    private:
        StreamResult prefetchAttributes(const char *name = nullptr);
        StreamResult skipValue();

    private:
        size_t mLevel = 0;
        bool mCurrentExtended = false;
        size_t mCurrentExtendedCount = 0;
        std::istream::pos_type mExtendedLookupPos = -1;
        std::map<std::string, std::istream::pos_type, std::less<>> mPrefetchedAttributes;
    };

}
}