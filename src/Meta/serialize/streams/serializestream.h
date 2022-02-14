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

        StreamResult readN(std::string &buffer, size_t n);
        StreamResult peekN(std::string &buffer, size_t n);
        StreamResult readUntil(std::string &buffer, const char *delim);
        StreamResult peekUntil(std::string &buffer, const char *delim);

        StreamResult read(void *buffer, size_t size);
        template <typename T>
        StreamResult read(T &t)
        {
            return read(&t, sizeof(T));
        }
        template <typename T>
        StreamResult operator>>(T &t)
        {
            InStream::operator>>(t);
            if (!*this)
                return STREAM_PARSE_ERROR(*this, true, "Unexpected EOF");
            return {};
        }

        SerializeStreamData &data() const;
        SerializeManager *manager() const;
        void setId(ParticipantId id);
        ParticipantId id() const;
        bool isMaster();
        SerializableUnitList &serializableList();

        template <typename T>
        requires std::convertible_to<T *, SerializableDataUnit *>
            StreamResult read(T *&p)
        {
            if constexpr (std::convertible_to<T *, SyncableUnitBase *>) {
                SyncableUnitBase *unit;
                STREAM_PROPAGATE_ERROR(read(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            } else {
                SerializableDataUnit *unit;
                STREAM_PROPAGATE_ERROR(read(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            }
            return {};
        }

        template <typename T>
        requires std::convertible_to<T *, SerializableDataUnit *>
            StreamResult operator>>(T *&p)
        {
            if constexpr (std::convertible_to<T *, SyncableUnitBase *>) {
                SyncableUnitBase *unit;
                STREAM_PROPAGATE_ERROR(operator>>(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            } else {
                SerializableDataUnit *unit;
                STREAM_PROPAGATE_ERROR(operator>>(unit));
                p = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(unit));
            }
            return {};
        }

        StreamResult read(SyncableUnitBase *&p);
        StreamResult read(SerializableDataUnit *&p);

        StreamResult operator>>(SyncableUnitBase *&p);
        StreamResult operator>>(SerializableDataUnit *&p);

        StreamResult read(std::string &s);
        StreamResult read(String auto &s)
        {
            std::string string;
            STREAM_PROPAGATE_ERROR(read(string));
            s = std::move(string);
            return {};
        }

        StreamResult operator>>(std::string &s);
        StreamResult operator>>(String auto &s)
        {
            std::string string;
            STREAM_PROPAGATE_ERROR(read(string));
            s = std::move(string);
            return {};
        }

        StreamResult read(ByteBuffer &b);

        StreamResult operator>>(ByteBuffer &b);

        StreamResult read(std::monostate &);

        StreamResult operator>>(std::monostate &);

    protected:
        SerializeInStream(std::basic_streambuf<char> *buffer, SerializeStreamData *data);

    protected:
        SerializeStreamData *mData;
    };

    struct META_EXPORT SerializeOutStream : OutStream {
    public:
        SerializeOutStream();
        SerializeOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data);
        SerializeOutStream(SerializeOutStream &&other);
        SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr);
        ~SerializeOutStream();

        SerializeStreamData &data() const;
        SerializeManager *manager() const;
        ParticipantId id() const;
        bool isMaster();
        SerializableUnitMap &serializableMap();

        template <typename T>
        requires(!Pointer<T> && !StringViewable<T>) void write(const T &t)
        {
            OutStream::write(t);
        }

        template <typename T>
        requires(!Pointer<T> && !StringViewable<T>) SerializeOutStream &operator<<(const T &t)
        {
            OutStream::operator<<(t);
            return *this;
        }

        void write(const SyncableUnitBase *p);
        void write(const SerializableDataUnit *p);

        SerializeOutStream &operator<<(const SyncableUnitBase *);
        SerializeOutStream &operator<<(const SerializableDataUnit *);

        void write(const std::string_view &s);
        void write(const StringViewable auto &s)
        {
            write(std::string_view { s });
        }

        SerializeOutStream &operator<<(const std::string_view &);
        SerializeOutStream &operator<<(const StringViewable auto &s)
        {
            return operator<<(std::string_view { s });
        }

        void write(const ByteBuffer &b);

        SerializeOutStream &operator<<(const ByteBuffer &);

        void write(const std::monostate &);

        SerializeOutStream &operator<<(const std::monostate &);

    protected:
        std::unique_ptr<SerializeStreamData> mData;
    };
}
}
