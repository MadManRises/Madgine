#pragma once

#include "../container/unithelper.h"
#include "../formatter.h"
#include "../primitivetypes.h"
#include "../serializeexception.h"
#include "../statetransmissionflags.h"
#include "Interfaces/streams/streams.h"
#include "debugging/streamdebugging.h"
#include "../../generic/iterator_traits.h"
#include "../container/creationhelper.h"
#include "../../generic/container_traits.h"

namespace Engine {
namespace Serialize {

    

    struct MODULES_EXPORT SerializeInStream : InStream {
    public:
        SerializeInStream();
        SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer);
        SerializeInStream(SerializeInStream &&other);
        SerializeInStream(SerializeInStream &&other, SerializeManager *mgr);

        template <typename T, typename = std::enable_if_t<isPrimitiveType_v<T> || std::is_base_of<SerializableBase, T>::value || std::is_base_of<SerializableUnitBase, T>::value || is_iterable_v<T>>>
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
            } else if constexpr (is_iterable_v<T>) {
                readContainer(t, name, std::forward<Args>(args)...);
            } else {
                static_assert(dependent_bool<T, false>::value, "Invalid Type");
            }
        }

        void read(ValueType &result, const char *name);

		template <typename C, typename Creator = DefaultCreator<>>
        void readContainer(C &container, const char *name = nullptr, Creator &&creator = {})
        {
            using T = typename C::value_type;

			container.clear();

            if (name)
                format().beginExtendedCompound(*this, name);
            decltype(container.size()) size;
            read(size, "size");
            if (name)
                format().beginCompound(*this, name);
            while (size--){
				//TODO use creator
                UnitHelper<T>::beginExtendedItem(*this, nullref<T>);
				auto pib = container_traits<C>::emplace(container, container.end());
                assert(pib.second);
                auto &t = *pib.first;                    
                read(t, "Item");
            }
            if (name)
                format().endCompound(*this, name);
        }

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
        void readUnformatted(Filesystem::Path &p);

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
                mLog.log(t);
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
            using T = typename C::value_type;

            if (name)
                format().beginExtendedCompound(*this, name);
            write(container.size(), "size");
            if (name)
                format().beginCompound(*this, name);
            for (const auto &t : container) {
                if (UnitHelper<T>::filter(*this, t)) {
                    UnitHelper<T>::beginExtendedItem(*this, t);
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
        void writeUnformatted(const Filesystem::Path &p);

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
