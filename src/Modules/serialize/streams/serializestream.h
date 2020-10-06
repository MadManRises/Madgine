#pragma once

#include "Interfaces/streams/streams.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream();
        SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager *mgr);

        template <typename T>
        SerializeInStream &operator>>(T &t);

        template <typename T>
        void readUnformatted(T &t)
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
        }

        template <typename T>
        void readUnformatted(T *&p)
        {
            static_assert(std::is_base_of_v<SerializableUnitBase, T>);

            if constexpr (std::is_base_of_v<SyncableUnitBase, T>) {
                SyncableUnitBase *unit;
                readUnformatted(unit);
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            } else {
                SerializableUnitBase *unit;
                readUnformatted(unit);
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            }
        }

        void readUnformatted(SyncableUnitBase *&p);
        void readUnformatted(SerializableUnitBase *&p);

        void readUnformatted(std::string &s);
        void readUnformatted(std::string_view &s) = delete;
        void readUnformatted(Filesystem::Path &p);

        void readUnformatted(ByteBuffer &b);

        void readUnformatted(std::monostate &);

        std::string readN(size_t n);
        std::string peekN(size_t n);
        std::string readUntil(const char *set);
        std::string peekUntil(const char *set);

        void readRaw(void *buffer, size_t size);
        template <typename T>
        void readRaw(T &t)
        {
            readRaw(&t, sizeof(T));
        }

        SerializeManager *manager() const;

        void setId(ParticipantId id);

        ParticipantId id() const;

        Formatter &format() const;
        bool isBinary() const;

        bool isMaster();

        SerializeStreambuf &buffer() const;

        void setNextFormattedStringDelimiter(char c);

        SerializableUnitMap &serializableMap();
        void startSerializableRead(SerializableMapHolder *map);

    protected:
        SerializeInStream(SerializeStreambuf *buffer);

    protected:
        char mNextFormattedStringDelimiter = 0;
    };

    struct MODULES_EXPORT SerializeOutStream : OutStream {
    public:
        SerializeOutStream();
        SerializeOutStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeOutStream(SerializeOutStream &&other);
        SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr);

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
        void writeUnformatted(const SerializableUnitBase *p);

        void writeUnformatted(const std::string &s);
        // void writeUnformatted(const std::string_view &s);
        void writeUnformatted(const Filesystem::Path &p);

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

        SerializeStreambuf &buffer() const;

    };

}
}

