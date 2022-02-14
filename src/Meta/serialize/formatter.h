#pragma once

#include "streams/streamresult.h"
#include "primitivetypes.h"

#include "streams/serializestream.h"

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

        virtual void beginExtendedWrite(const char *name, size_t count) {};

        virtual void beginCompoundWrite(const char *name) {};
        virtual void endCompoundWrite(const char *name) {};

        virtual StreamResult beginExtendedRead(const char *name, size_t count);

        virtual StreamResult beginCompoundRead(const char *name);
        virtual StreamResult endCompoundRead(const char *name);

        virtual void beginPrimitiveWrite(const char *name, uint8_t typeId) { }
        virtual void endPrimitiveWrite(const char *name, uint8_t typeId) { }

        virtual StreamResult beginPrimitiveRead(const char *name, uint8_t typeId);
        virtual StreamResult endPrimitiveRead(const char *name, uint8_t typeId);

        virtual void beginContainerWrite(const char *name, uint32_t size = std::numeric_limits<uint32_t>::max()) { }
        virtual void endContainerWrite(const char *name) { }

        virtual StreamResult beginContainerRead(const char *name, bool sized = true);
        virtual StreamResult endContainerRead(const char *name);

        virtual bool hasContainerItem() = 0;

        virtual StreamResult lookupFieldName(std::string &name)
        {
            return STREAM_INTEGRITY_ERROR(mIn, mBinary, "Name lookup is not implemented in Formatter!");
        }

        
        template <typename T>
        StreamResult read(T &t)
        {
            if (mBinary) {
                return mIn.read(t);
            } else {
                return mIn >> t;
            }
        }

        StreamResult read(std::string &s);

        StreamResult read(String auto &s)
        {
            std::string string;
            STREAM_PROPAGATE_ERROR(read(string));
            s = std::move(string);
            return {};
        }

        template <typename T>
        void write(const T &t)
        {
            if (mBinary) {
                mOut.write(t);
            } else {
                mOut << t;
            }
        }


        StreamResult expect(std::string_view expected);


        const bool mBinary;
        const bool mSupportNameLookup;

    protected:
        const char *mNextStringDelimiter = nullptr;

        SerializeInStream mIn;
        SerializeOutStream mOut;
    };

    #define FORMATTER_EXPECT(text) STREAM_PROPAGATE_ERROR(expect(text))

}
}