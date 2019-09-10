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

        virtual void beginMember(SerializeOutStream &, const char *name, bool first) {};
        virtual void endMember(SerializeOutStream &, const char *name, bool first) {};

        virtual void beginMember(SerializeInStream &, const char *name, bool first) {};
        virtual void endMember(SerializeInStream &, const char *name, bool first) {};

		virtual void beginPrimitive(SerializeOutStream &, size_t typeId) {}
        virtual void endPrimitive(SerializeOutStream &, size_t typeId) {}

		virtual void beginPrimitive(SerializeInStream &, size_t typeId) {}
        virtual void endPrimitive(SerializeInStream &, size_t typeId) {}

		virtual void writeEOL(SerializeOutStream &) = 0;
        virtual bool readEOL(SerializeInStream &) = 0;

        virtual std::string lookupFieldName(SerializeInStream &)
        {
            return {};
        }

        const bool mBinary;
        const bool mSupportNameLookup;
    };

}
}