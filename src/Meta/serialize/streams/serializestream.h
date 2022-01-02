#pragma once

#include "Generic/streams.h"

#include "streamresult.h"

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
        SerializeInStream &operator>>(T &t) = delete;

        template <typename T>
        StreamResult readUnformatted(T &t)
        {
            if (isBinary()) {
                return readRaw(t);
            } else {

                InStream::operator>>(t);
                if (!*this)
                    return STREAM_PARSE_ERROR(*this, "Expected: <" << typeName<T>() << ">");
                return {};
            }
        }

        template <typename T>
        requires std::convertible_to<T *, SerializableDataUnit *>
        StreamResult readUnformatted(T *&p) 
        {
            if constexpr (std::convertible_to<T *, SyncableUnitBase *>) {
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
        StreamResult readUnformatted(String auto& s) {
            std::string string;
            STREAM_PROPAGATE_ERROR(readUnformatted(string));
            s = std::move(string);
            return {};
        }

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
        SerializeOutStream &operator<<(const T &t) = delete;

        template <typename T>
        requires(!Pointer<T> && !StringViewable<T>) void writeUnformatted(const T &t)
        {
            if (isBinary())
                writeRaw(t);
            else
                OutStream::operator<<(t);
        }

        void writeUnformatted(const SyncableUnitBase *p);
        void writeUnformatted(const SerializableDataUnit *p);

        void writeUnformatted(const std::string_view &s);
        void writeUnformatted(const StringViewable auto& s) {
            writeUnformatted(std::string_view { s });
        }

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

    protected:
        std::unique_ptr<SerializeStreamData> mData;
    };

}
}
