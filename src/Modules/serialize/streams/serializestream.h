#pragma once

#include "../../generic/templates.h"
#include "../serializeexception.h"
#include "Interfaces/streams/streams.h"
#include "debugging/streamdebugging.h"

namespace Engine {
namespace Serialize {

    class EOLType {
    public:
        constexpr bool operator==(const EOLType &) const { return true; }
    };

    constexpr const int SERIALIZE_MAGIC_NUMBER = 0x12345678;

    using SerializePrimitives = type_pack<bool, size_t, int, float, SerializableUnitBase *, EOLType, std::string, Vector2, Vector3, InvScopePtr>;

    template <class T, class = void>
    struct PrimitiveTypeIndex : type_pack_index<SerializePrimitives, T> {
    };

    template <class T>
    struct PrimitiveTypeIndex<T, std::enable_if_t<std::is_enum_v<T>>> : PrimitiveTypeIndex<int> {
    };

    template <class T>
    const constexpr size_t PrimitiveTypeIndex_v = PrimitiveTypeIndex<T>::value;

    template <class T, class = void>
    struct PrimitiveTypesContain : type_pack_contains<SerializePrimitives, T> {
    };

    template <class T>
    struct PrimitiveTypesContain<T, std::enable_if_t<std::is_enum_v<T>>> : PrimitiveTypesContain<int> {
    };

    template <class T>
    const constexpr bool PrimitiveTypesContain_v = PrimitiveTypesContain<T>::value;

    struct MODULES_EXPORT SerializeStreambuf : std::basic_streambuf<char> {
    public:
        SerializeStreambuf() = default;
        SerializeStreambuf(SerializeManager &mgr, ParticipantId id);
        virtual ~SerializeStreambuf() = default;

        void setManager(SerializeManager *mgr);
        SerializeManager *manager();
        bool isMaster();

        ParticipantId id() const;
        void setId(ParticipantId id);

    private:
        SerializeManager *mManager = nullptr;
        ParticipantId mId = 0;
    };

    template <typename Buf>
    struct WrappingSerializeStreambuf : SerializeStreambuf, Buf {
        using Buf::Buf;

        void imbue(const std::locale &loc) override
        {
            Buf::imbue(loc);
        }

        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode mode = std::ios_base::in) override
        {
            return Buf::seekoff(off, dir, mode);
        }
        pos_type seekpos(pos_type pos,
            std::ios_base::openmode mode = std::ios_base::in) override
        {
            return Buf::seekpos(pos, mode);
        }

        std::streamsize showmanyc() override
        {
            return Buf::showmanyc();
        }

        std::streamsize xsgetn(char *s, std::streamsize n) override
        {
            return Buf::xsgetn(s, n);
        }

		int pbackfail(int c = EOF) override {
                    return Buf::pbackfail(c);
		}

        int_type overflow(int c = EOF) override
        {
            return Buf::overflow(c);
        }

		std::streamsize xsputn(const char* s, std::streamsize n) override {
                    return Buf::xsputn(s, n);
		}

        int_type underflow() override
        {
            return Buf::underflow();
        }

		int uflow() override {
                    return Buf::uflow();
		}

        int sync() override
        {
            return Buf::sync();
        }
    };

    struct MODULES_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager *mgr);

        template <typename T>
        SerializeInStream &operator>>(T &t)
        {
            read(t);
            return *this;
        }

        template <class T, typename... Args, typename = std::enable_if_t<PrimitiveTypesContain_v<T> || std::is_base_of<SerializableBase, T>::value>>
        void read(T &t, Args &&... args)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                int type;
                readRaw(type);
                if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<T>)
                    throw SerializeException(Database::Exceptions::unknownSerializationType);
                readRaw(t);
                mLog.log(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
                t.readState(*this, std::forward<Args>(args)...);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        void read(ValueType &result);

        template <class T, typename V = std::enable_if_t<std::is_base_of<SerializableUnitBase, T>::value>>
        void read(T *&p)
        {
            SerializableUnitBase *unit;
            *this >> unit;
            p = dynamic_cast<T *>(unit);
            if (unit && !p)
                throw 0;
        }

        void read(SerializableUnitBase *&p);

        void read(std::string &s);

        template <class T>
        bool loopRead(T &val)
        {
            bool result = loopRead();
            if (result)
                *this >> val;
            return result;
        }

        bool loopRead();

        template <typename T>
        SerializeInStream &readPlain(T &t, Formatter &format)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                InStream::operator>>(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
                t.readStatePlain(*this, format);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
            return *this;
        }

		std::string readPlainN(size_t n);
        std::string readPlainUntil(char c);
        std::string peekPlainUntil(char c);

        bool loopReadPlain(Formatter &format);

        void readRaw(void *buffer, size_t size);

        void logReadHeader(const MessageHeader &header, const std::string &object);

        SerializeManager *manager() const;

        void setId(ParticipantId id);

        ParticipantId id() const;

        bool isMaster();

    protected:
        SerializeInStream(SerializeStreambuf *buffer);

        template <class T>
        void readRaw(T &t)
        {
            readRaw(&t, sizeof(T));
        }

        SerializableUnitBase *convertPtr(size_t ptr);

        SerializeStreambuf &buffer() const;

    protected:
        Debugging::StreamLog mLog;
    };

    struct MODULES_EXPORT SerializeOutStream : OutStream {
    public:
        SerializeOutStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeOutStream(SerializeOutStream &&other);
        SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr);

        ParticipantId id() const;

        SerializeOutStream &operator<<(const ValueType &v);

        template <class T, typename = std::enable_if_t<PrimitiveTypesContain_v<T> || std::is_base_of<SerializableBase, T>::value>>
        SerializeOutStream &operator<<(const T &t)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                writeRaw<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<T>);
                writeRaw(t);
                mLog.log(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
                t.writeState(*this);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
            return *this;
        }

        SerializeOutStream &operator<<(SerializableUnitBase *p);

        SerializeOutStream &operator<<(const std::string &s);

        template <typename T>
        SerializeOutStream &writePlain(const T &t, Formatter &format)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                OutStream::operator<<(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
                t.writeStatePlain(*this, format);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
            return *this;
        }

        void writeRaw(const void *buffer, size_t size);

        SerializeManager *manager() const;

        bool isMaster();

    protected:
        pos_type tell() const;
        void seek(pos_type p);

        template <class T>
        void writeRaw(const T &t)
        {
            writeRaw(&t, sizeof(T));
        }

        SerializeStreambuf &buffer() const;

    protected:
        Debugging::StreamLog mLog;
    };
}
} // namespace Scripting
