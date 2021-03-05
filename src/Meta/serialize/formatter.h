#pragma once

namespace Engine {
namespace Serialize {

    struct META_EXPORT Formatter {

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

		virtual StreamResult beginExtended(Serialize::SerializeInStream &, const char *name, size_t count);

        virtual StreamResult beginCompound(Serialize::SerializeInStream &, const char *name);
        virtual StreamResult endCompound(Serialize::SerializeInStream &, const char *name);

        virtual void beginPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) {}
        virtual void endPrimitive(SerializeOutStream &, const char *name, uint8_t typeId) {}

        virtual StreamResult beginPrimitive(SerializeInStream &, const char *name, uint8_t typeId);
        virtual StreamResult endPrimitive(SerializeInStream &, const char *name, uint8_t typeId);

        virtual void beginContainer(SerializeOutStream &, const char *name, uint32_t size = std::numeric_limits<uint32_t>::max()) { }
        virtual void endContainer(SerializeOutStream &, const char *name) { }

        virtual StreamResult beginContainer(SerializeInStream &, const char *name, bool sized = true);
        virtual StreamResult endContainer(SerializeInStream &, const char *name);

        virtual bool hasContainerItem(SerializeInStream &) = 0;

        virtual std::string lookupFieldName(SerializeInStream &)
        {
            return {};
        }

        const bool mBinary;
        const bool mSupportNameLookup;
    };

}
}