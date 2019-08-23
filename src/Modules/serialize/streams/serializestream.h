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
        SerializeStreambuf(SerializeManager &mgr, ParticipantId id);
        virtual ~SerializeStreambuf() = default;

        void setManager(SerializeManager &mgr);
        SerializeManager &manager();
        bool isMaster();

        ParticipantId id() const;
        void setId(ParticipantId id);

    private:
        SerializeManager *mManager;
        ParticipantId mId;
    };

    struct MODULES_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager &mgr);

		template <typename T>
		SerializeInStream& operator>>(T& t) {
            read(t);
                    return *this;
		}

        template <class T, typename... Args, typename = std::enable_if_t<PrimitiveTypesContain_v<T> || std::is_base_of<Serializable, T>::value>>
        void read(T &t, Args&&... args)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                int type;
                readRaw(type);
                if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<T>)
                    throw SerializeException(Database::Exceptions::unknownSerializationType);
                readRaw(t);
                mLog.log(t);
            } else if constexpr (std::is_base_of<Serializable, T>::value) {
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

        void readRaw(void *buffer, size_t size);

        void logReadHeader(const MessageHeader &header, const std::string &object);

        SerializeManager &manager() const;

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
        SerializeOutStream(SerializeOutStream &&other, SerializeManager &mgr);

        ParticipantId id() const;

        SerializeOutStream &operator<<(const ValueType &v);

        template <class T, typename = std::enable_if_t<PrimitiveTypesContain_v<T> || std::is_base_of<Serializable, T>::value>>
        SerializeOutStream &operator<<(const T &t)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                write<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<T>);
                write(t);
                mLog.log(t);
            } else if constexpr (std::is_base_of<Serializable, T>::value) {
                t.writeState(*this);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
            return *this;
        }

        SerializeOutStream &operator<<(SerializableUnitBase *p);

        SerializeOutStream &operator<<(const std::string &s);

        void writeRaw(const void *buffer, size_t size);

        SerializeManager &manager() const;

        bool isMaster();

    protected:
        pos_type tell() const;
        void seek(pos_type p);

        template <class T>
        void write(const T &t)
        {
            writeData(&t, sizeof(T));
        }

        void writeData(const void *data, size_t count);

        SerializeStreambuf &buffer() const;

    protected:
        Debugging::StreamLog mLog;
    };
}
} // namespace Scripting
