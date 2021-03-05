#pragma once

#include "Generic/streams.h"

#include "../serializeexception.h"

#include "streamerror.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream();
        SerializeInStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager *mgr);
        ~SerializeInStream();

        template <typename T>
        SerializeInStream &operator>>(T &t);

        template <typename T>
        StreamResult readUnformatted(T &t)
        {
            if constexpr (std::is_enum_v<T>) {
                int buffer;
                readUnformatted(buffer);
                t = static_cast<T>(buffer);
            } else {
                if (isBinary())
                    readRaw(t);
                else
                    InStream::operator>>(t);
            }
            if (!*this)
                return STREAM_PARSE_ERROR(*this, "Expected: <" << typeName<T>() << ">");
            return {};
        }

        template <typename T>
        StreamResult readUnformatted(T *&p)
        {
            static_assert(std::is_base_of_v<SerializableUnitBase, T>);

            if constexpr (std::is_base_of_v<SyncableUnitBase, T>) {
                SyncableUnitBase *unit;
                STREAM_PROPAGATE_ERROR(readUnformatted(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));                
            } else {
                SerializableDataUnit *unit;
                STREAM_PROPAGATE_ERROR(readUnformatted(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            }
            return {};
        }

        StreamResult readUnformatted(SyncableUnitBase *&p);
        StreamResult readUnformatted(SerializableDataUnit *&p);

        StreamResult readUnformatted(std::string &s);
        StreamResult readUnformatted(std::string_view &s) = delete;

        StreamResult readUnformatted(ByteBuffer &b);

        StreamResult readUnformatted(std::monostate &);

        std::string readN(size_t n);
        std::string peekN(size_t n);
        std::string readUntil(const char *set);
        std::string peekUntil(const char *set);

        StreamResult readRaw(void *buffer, size_t size);
        template <typename T>
        StreamResult readRaw(T &t)
        {
            return readRaw(&t, sizeof(T));
        }

        SerializeManager *manager() const;

        void setId(ParticipantId id);

        ParticipantId id() const;

        Formatter &format() const;
        bool isBinary() const;

        bool isMaster();

        SerializeStreamData &data() const;

        void setNextFormattedStringDelimiter(char c);

        SerializableUnitList &serializableList();
        void startSerializableRead(SerializableListHolder *list);

    protected:
        SerializeInStream(std::basic_streambuf<char> *buffer, SerializeStreamData *data);

    protected:
        char mNextFormattedStringDelimiter = 0;
        SerializeStreamData *mData;
    };

    struct META_EXPORT SerializeOutStream : OutStream {
    public:
        SerializeOutStream();
        SerializeOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data);
        SerializeOutStream(SerializeOutStream &&other);
        SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr);
        ~SerializeOutStream();

        ParticipantId id() const;

        template <typename T>
        SerializeOutStream &operator<<(const T &t);

        SerializeOutStream &operator<<(const char *s);

        template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
        void writeUnformatted(const T &t)
        {
            if constexpr (std::is_enum_v<T>) {
                writeUnformatted(static_cast<int>(t));
            } else {
                if (isBinary())
                    writeRaw(t);
                else
                    OutStream::operator<<(t);
            }
        }

        void writeUnformatted(const SyncableUnitBase *p);
        void writeUnformatted(const SerializableDataUnit *p);

        void writeUnformatted(const std::string &s);
        void writeUnformatted(const std::string_view &s);

        void writeUnformatted(const ByteBuffer &b);

        void writeUnformatted(const std::monostate &);

        void writeRaw(const void *buffer, size_t size);
        template <typename T>
        void writeRaw(const T &t)
        {
            writeRaw(&t, sizeof(T));
        }

        SerializeManager *manager() const;

        Formatter &format() const;
        bool isBinary() const;

        bool isMaster();

        SerializeStreamData &data() const;

        SerializableUnitMap &serializableMap();
        void startSerializableWrite(SerializableMapHolder *map);

    protected:
        std::unique_ptr<SerializeStreamData> mData;
    };

}
}
