#pragma once

#include "../creationhelper.h"
#include "../formatter.h"
#include "../primitivetypes.h"
#include "../serializeexception.h"
#include "../statetransmissionflags.h"
#include "../unithelper.h"
#include "Interfaces/streams/streams.h"
#include "debugging/streamdebugging.h"

namespace Engine {
namespace Serialize {

    template <typename... Ty>
    void read(SerializeInStream &in, std::tuple<Ty &...> t, const char *name = nullptr)
    {
        in.format().beginCompound(in, name);
        TupleUnpacker::forEach(t, [&](auto &e) { read(in, e); });
        in.format().endCompound(in, name);
    }

    template <typename... Ty>
    void read(SerializeInStream &in, std::tuple<Ty...> &t, const char *name = nullptr)
    {
        in.format().beginCompound(in, name);
        TupleUnpacker::forEach(t, [&](auto &e) { read(in, e); });
        in.format().endCompound(in, name);
    }

    //void read(ValueType &result, const char *name);

    template <typename C, typename Creator = DefaultCreator<>>
    void readContainer(SerializeInStream &in, C &container, const char *name = nullptr, Creator &&creator = {})
    {
        using T = typename C::value_type;

        in.format().beginExtended(in, name);
        decltype(container.size()) size;
        read(in, size, "size");
        in.format().beginCompound(in, name);

        if constexpr (!container_traits<C>::is_fixed_size) {
            container.clear();

            while (size--) {
                //TODO use creator
                UnitHelper<T>::beginExtendedItem(in, nullref<T>);
                auto pib = container_traits<C>::emplace(container, container.end());
                assert(pib.second);
                auto &t = *pib.first;
                read(in, t, "Item");
            }
        } else {
            assert(container.size() == size);
            for (T &t : container) {
                read(in, t, "Item");
            }
        }

        in.format().endCompound(in, name);
    }

    template <typename T, typename... Args>
    void read(SerializeInStream &in, T &t, const char *name, Args &&... args)
    {
        if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            in.format().beginPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            in.readUnformatted(t);
            in.format().endPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (std::is_base_of<SerializableBase, T>::value) {
            t.readState(in, name, std::forward<Args>(args)...);
        } else if constexpr (std::is_base_of<SerializableUnitBase, T>::value) {
            t.readState(in, name, StateTransmissionFlags_DontApplyMap);
        } else if constexpr (is_instance_v<T, std::unique_ptr>) {
            read(in, *t, name, std::forward<Args>(args)...);
        } else if constexpr (is_iterable_v<T>) {
            readContainer(in, t, name, std::forward<Args>(args)...);
        } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            read(in, TupleUnpacker::toTuple(t), name);
        } else {
            static_assert(dependent_bool<T, false>::value, "Invalid Type");
        }
    }

    template <typename T>
    void read(SerializeInStream &in, T &t)
    {
        read(in, t, nullptr);
    }

    template <typename T>
    void write(SerializeOutStream &out, const T &t, const char *name = nullptr)
    {
        if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (std::is_base_of<SerializableBase, T>::value || std::is_base_of<SerializableUnitBase, T>::value) {
            t.writeState(out, name);
        } else if constexpr (is_instance_v<T, std::unique_ptr>) {
            write(out, *t, name);
        } else if constexpr (is_iterable_v<T>) {
            writeContainer(out, t, name);
        } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } else {
            static_assert(dependent_bool<T, false>::value, "Invalid Type");
        }
    }

    template <typename... Ty>
    void write(SerializeOutStream &out, const std::tuple<Ty...> &t, const char *name = nullptr)
    {
        out.format().beginCompound(out, name);
        TupleUnpacker::forEach(t, [&](const auto &e) { write(out, e, "Element"); });
        out.format().endCompound(out, name);
    }

    template <typename C>
    void writeContainer(SerializeOutStream &out, const C &container, const char *name = nullptr)
    {
        using T = typename C::value_type;

        out.format().beginExtended(out, name);
        write(out, container.size(), "size");
        out.format().beginCompound(out, name);
        for (const auto &t : container) {
            if (UnitHelper<T>::filter(out, t)) {
                UnitHelper<T>::beginExtendedItem(out, t);
                UnitHelper<T>::write_creation(out, t);
                write(out, t, "Item");
            }
        }
        out.format().endCompound(out, name);
    }

    struct MODULES_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream();
        SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager *mgr);

        template <typename T>
        SerializeInStream &operator>>(T &t)
        {
            read(*this, t);
            return *this;
        }

        template <typename T>
        void readUnformatted(T &t)
        {
            if constexpr (std::is_enum_v<T>) {
                int buffer;
                readUnformatted(buffer);
                t = static_cast<T>(buffer);
            } else {
                if (format().mBinary)
                    readRaw(t);
                else
                    InStream::operator>>(t);
            }
        }

        template <typename T>
        void readUnformatted(T *&p)
        {
            static_assert(std::is_base_of<SerializableUnitBase, T>::value);

            SerializableUnitBase *unit;
            readUnformatted(unit);
            p = reinterpret_cast<T *>(reinterpret_cast<size_t>(unit));
        }

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

        void logReadHeader(const MessageHeader &header, const std::string &object);

        SerializeManager *manager() const;

        void setId(ParticipantId id);

        ParticipantId id() const;

        Formatter &format() const;

        bool isMaster();

        SerializeStreambuf &buffer() const;

        void setNextFormattedStringDelimiter(char c);

        SerializableUnitBase *convertPtr(size_t ptr);

    protected:
        SerializeInStream(SerializeStreambuf *buffer);

    protected:
        Debugging::StreamLog mLog;

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
        SerializeOutStream &operator<<(const T &t)
        {
            write(*this, t);
            return *this;
        }

        SerializeOutStream &operator<<(const char *s)
        {
            write(*this, std::string(s));
            return *this;
        }

        template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
        void writeUnformatted(const T &t)
        {
            if constexpr (std::is_enum_v<T>) {
                writeUnformatted(static_cast<int>(t));
            } else {
                if (format().mBinary)
                    writeRaw(t);
                else
                    OutStream::operator<<(t);
            }
        }

        void writeUnformatted(SerializableUnitBase *p);

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

        bool isMaster();

        SerializeStreambuf &buffer() const;

    protected:
        Debugging::StreamLog mLog;
    };

}
}
