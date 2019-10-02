#pragma once

#include "../formatter.h"
#include "../serializeexception.h"
#include "../serializetable.h"
#include "Interfaces/streams/streams.h"
#include "debugging/streamdebugging.h"
#include "../../keyvalue/container_traits.h"
#include "../statesubmissionflags.h"
#include "../primitivetypes.h"
#include "../container/unithelper.h"

namespace Engine {
namespace Serialize {

    enum class StreamMode {
        READ,
        WRITE
    };

    struct MODULES_EXPORT SerializeStreambuf : std::basic_streambuf<char> {
    public:
        SerializeStreambuf(std::unique_ptr<Formatter> format);
        SerializeStreambuf(SerializeStreambuf &&) = default;
        SerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id);
        virtual ~SerializeStreambuf() = default;

        void setManager(SerializeManager *mgr);
        SerializeManager *manager();
        bool isMaster(StreamMode mode);

        ParticipantId id() const;
        void setId(ParticipantId id);

        Formatter &format() const;

    private:
        SerializeManager *mManager = nullptr;
        ParticipantId mId = 0;
        std::unique_ptr<Formatter> mFormatter;
    };

    template <typename Buf>
    struct WrappingSerializeStreambuf : SerializeStreambuf, Buf {

		WrappingSerializeStreambuf(std::unique_ptr<Formatter> format)
            : SerializeStreambuf(std::move(format))
        {
        }

        template <typename... Args, typename = std::enable_if_t<!std::is_convertible_v<typename first<Args...>::type, SerializeManager &>>>
        WrappingSerializeStreambuf(std::unique_ptr<Formatter> format, Args &&... args)
            : SerializeStreambuf(std::move(format))
            , Buf(std::forward<Args>(args)...)
        {
        }

		template <typename... Args>
        WrappingSerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id, Args &&... args)
            : SerializeStreambuf(std::move(format), mgr, id)
            , Buf(std::forward<Args>(args)...)
        {
        }

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

        int pbackfail(int c = EOF) override
        {
            return Buf::pbackfail(c);
        }

        int_type overflow(int c = EOF) override
        {
            return Buf::overflow(c);
        }

        std::streamsize xsputn(const char *s, std::streamsize n) override
        {
            return Buf::xsputn(s, n);
        }

        int_type underflow() override
        {
            return Buf::underflow();
        }

        int uflow() override
        {
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

        template <typename T, typename = std::enable_if_t<isPrimitiveType_v<T> || std::is_base_of<SerializableBase, T>::value || std::is_base_of<SerializableUnitBase, T>::value>>
        SerializeInStream &operator>>(T &t)
        {
            read(t);
            return *this;
        }

        template <typename T>
        void read(T &t)
        {
            read(t, nullptr);
        }

        template <class T, typename... Args>
        void read(T &t, const char *name, Args &&... args)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                format().beginPrimitive(*this, name, PrimitiveTypeIndex_v<T>);
                readUnformatted(t);
                format().endPrimitive(*this, name, PrimitiveTypeIndex_v<T>);
                mLog.log(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
                t.readState(*this, name, std::forward<Args>(args)...);
            } else if constexpr (std::is_base_of<SerializableUnitBase, T>::value) {
                t.readState(*this, name, StateTransmissionFlags_DontApplyMap);
            } else if constexpr (std::is_enum_v<T>) {
                int dummy;
                read(dummy, name, std::forward<Args>(args)...);
                t = static_cast<T>(dummy);
            } else if constexpr (is_instance_v<T, std::unique_ptr>) {
                read(*t, name, std::forward<Args>(args)...);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        void read(ValueType &result, const char *name);

        template <typename T>
        void readUnformatted(T &t)
        {
            if (format().mBinary)
                readRaw(t);
            else
                InStream::operator>>(t);
        }

        template <class T, typename V = std::enable_if_t<std::is_base_of<SerializableUnitBase, T>::value>>
        void readUnformatted(T *&p)
        {
            SerializableUnitBase *unit;
            readUnformatted(unit);
            p = reinterpret_cast<T *>(reinterpret_cast<size_t>(unit));
        }

        void readUnformatted(SerializableUnitBase *&p);

        void readUnformatted(std::string &s);

        std::string readN(size_t n);
        std::string peekN(size_t n);
        std::string readUntil(const char *set);
        std::string peekUntil(const char *set);

        void readRaw(void *buffer, size_t size);
        template <class T>
        void readRaw(T &t)
        {
            readRaw(&t, sizeof(T));
        }

        void logReadHeader(const MessageHeader &header, const std::string &object);

        SerializeManager *manager() const;

        void setId(ParticipantId id);

        ParticipantId id() const;

        Formatter &format() const;

        bool isMaster();

		SerializeStreambuf &buffer() const;

    protected:
        SerializeInStream(SerializeStreambuf *buffer);

        SerializableUnitBase *convertPtr(size_t ptr);

    protected:
        Debugging::StreamLog mLog;
    };

    struct MODULES_EXPORT SerializeOutStream : OutStream {
    public:
        SerializeOutStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeOutStream(SerializeOutStream &&other);
        SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr);

        ParticipantId id() const;

        template <typename T, typename = std::enable_if_t<isPrimitiveType_v<T> || std::is_base_of<SerializableBase, T>::value || std::is_base_of<SerializableUnitBase, T>::value || is_iterable_v<T>>>
        SerializeOutStream &operator<<(const T &t)
        {
            write(t);
            return *this;
        }

        SerializeOutStream &operator<<(const char *s)
        {
            write(std::string(s));
            return *this;
        }

        template <class T>
        void write(const T &t, const char *name = nullptr)
        {
            if constexpr (PrimitiveTypesContain_v<T>) {
                format().beginPrimitive(*this, name, PrimitiveTypeIndex_v<T>);
                writeUnformatted(t);
                format().endPrimitive(*this, name, PrimitiveTypeIndex_v<T>);
                //mLog.log(t);
            } else if constexpr (std::is_base_of<SerializableBase, T>::value || std::is_base_of<SerializableUnitBase, T>::value) {
                t.writeState(*this, name);
            } else if constexpr (std::is_enum_v<T>) {
                write(static_cast<int>(t), name);
            } else if constexpr (is_instance_v<T, std::unique_ptr>) {
                write(*t, name);
            } else if constexpr (is_iterable_v<T>) {
                writeContainer(t, name);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        template <typename C>
        void writeContainer(const C &container, const char *name = nullptr)
        {
			using traits = container_traits<C>;
			using T = typename traits::type;

            if (name)
                format().beginExtendedCompound(*this, name);
            write(container.size(), "size");
            if (name)
                format().beginCompound(*this, name);
            for (const auto &t : container) {
                if (UnitHelper<T>::filter(*this, t)) {
                    if constexpr (UnitHelper<T>::sIsUnit) {
                        format().beginExtendedCompound(*this, "Item");
                    }
                    UnitHelper<T>::write_creation(*this, t);
                    write(t, "Item");
                }
            }
            if (name)
                format().endCompound(*this, name);
        }

        template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
        void writeUnformatted(const T &t)
        {
            if (format().mBinary)
                writeRaw(t);
            else
                OutStream::operator<<(t);
        }

        void writeUnformatted(SerializableUnitBase *p);
        void writeUnformatted(const std::string &s);

        void writeRaw(const void *buffer, size_t size);
        template <class T>
        void writeRaw(const T &t)
        {
            writeRaw(&t, sizeof(T));
        }

        SerializeManager *manager() const;

        Formatter &format() const;

        bool isMaster();
    
        SerializeStreambuf &buffer() const;

    protected:
        Debugging::StreamLog mLog;
    };
}
} // namespace Scripting
