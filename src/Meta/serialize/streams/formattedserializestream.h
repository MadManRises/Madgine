#pragma once

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    struct FormattedSerializeStream {

        void beginExtendedWrite(const char *name, size_t count);

        void beginCompoundWrite(const char *name);
        void endCompoundWrite(const char *name);

        StreamResult beginExtendedRead(const char *name, size_t count);

        StreamResult beginCompoundRead(const char *name);
        StreamResult endCompoundRead(const char *name);

        void beginPrimitiveWrite(const char *name, uint8_t typeId);
        void endPrimitiveWrite(const char *name, uint8_t typeId);

        StreamResult beginPrimitiveRead(const char *name, uint8_t typeId);
        StreamResult endPrimitiveRead(const char *name, uint8_t typeId);

        void beginContainerWrite(const char *name, uint32_t size = std::numeric_limits<uint32_t>::max());
        void endContainerWrite(const char *name);

        StreamResult beginContainerRead(const char *name, bool sized = true);
        StreamResult endContainerRead(const char *name);

        bool hasContainerItem();

        StreamResult lookupFieldName(std::string &name);

        bool supportsNameLookup() const;

        template <PrimitiveType T>
        void writePrimitive(const T &t, const char *name)
        {
            beginPrimitiveWrite(name, PrimitiveTypeIndex_v<T>);
            mFormatter->write(t);
            endPrimitiveWrite(name, PrimitiveTypeIndex_v<T>);
        }

        template <PrimitiveType T>
        StreamResult readPrimitive(T &t, const char *name)
        {
            STREAM_PROPAGATE_ERROR(beginPrimitiveRead(name, PrimitiveTypeIndex_v<T>));
            STREAM_PROPAGATE_ERROR(mFormatter->read(t));
            return endPrimitiveRead(name, PrimitiveTypeIndex_v<T>);
        }

        explicit operator bool() const;

        SerializeStreamData &data();

        bool isMaster();

        bool isBinary();

        void setId(ParticipantId id);
        ParticipantId id() const;

        SerializeManager *manager();

        SerializableUnitList &serializableList();

        SerializeInStream &in();
        SerializeOutStream &out();

        std::unique_ptr<Formatter> mFormatter;
    };

}
}