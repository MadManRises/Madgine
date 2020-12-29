#pragma once

#include "Modules/serialize/formatter.h"

namespace Engine {
namespace XML {

    struct MADGINE_XML_EXPORT XMLFormatter : Serialize::Formatter {

        XMLFormatter();

		virtual void setupStream(std::istream &) override;
        virtual void setupStream(std::ostream &) override;

        virtual void beginExtended(Serialize::SerializeOutStream &, const char *name, size_t count) override;

        virtual void beginCompound(Serialize::SerializeOutStream &, const char *name) override;
        virtual void endCompound(Serialize::SerializeOutStream &, const char *name) override;

		virtual void beginExtended(Serialize::SerializeInStream &, const char *name, size_t count) override;

        virtual void beginCompound(Serialize::SerializeInStream &, const char *name) override;
        virtual void endCompound(Serialize::SerializeInStream &, const char *name) override;

        virtual void beginPrimitive(Serialize::SerializeOutStream &, const char *name, uint8_t typeId) override;
        virtual void endPrimitive(Serialize::SerializeOutStream &, const char *name, uint8_t typeId) override;

        virtual void beginPrimitive(Serialize::SerializeInStream &, const char *name, uint8_t typeId) override;
        virtual void endPrimitive(Serialize::SerializeInStream &, const char *name, uint8_t typeId) override;

        virtual std::string lookupFieldName(Serialize::SerializeInStream &) override;

        std::string indent();

    private:
        size_t mLevel = 0;
        bool mCurrentExtended = false;
        size_t mCurrentExtendedCount = 0;
    };

}
}