#pragma once

namespace Engine {
namespace Serialize {

    struct Formatter {

        Formatter(bool isBinary, bool supportNameLookup = false)
            : mBinary(isBinary)
            , mSupportNameLookup(supportNameLookup)
        {
        }
        virtual ~Formatter() = default;

		virtual void setupStream(std::istream &) {};
        virtual void setupStream(std::ostream &) {}; 

        virtual void beginExtended(Serialize::SerializeOutStream &, const char *name, size_t count) {};

        virtual void beginCompound(Serialize::SerializeOutStream &, const char *name) {};
        virtual void endCompound(Serialize::SerializeOutStream &, const char *name) {};

		virtual void beginExtended(Serialize::SerializeInStream &, const char *name, size_t count) {};

        virtual void beginCompound(Serialize::SerializeInStream &, const char *name) {};
        virtual void endCompound(Serialize::SerializeInStream &, const char *name) {};

        virtual void beginPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) {}
        virtual void endPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) {}

        virtual void beginPrimitive(SerializeInStream &, const char *name, uint8_t typeId) {}
        virtual void endPrimitive(SerializeInStream &, const char *name, uint8_t typeId) {}

        virtual std::string lookupFieldName(SerializeInStream &)
        {
            return {};
        }

        const bool mBinary;
        const bool mSupportNameLookup;
    };

}
}