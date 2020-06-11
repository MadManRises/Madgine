#pragma once

#include "../container/physical.h"
#include "../creationhelper.h"
#include "serializestream.h"

namespace Engine {
namespace Serialize {

    DERIVE_TYPENAME(Config);
    DERIVE_TYPENAME(ResetOperation);
    DERIVE_FUNCTION_ARGS(writeState, SerializeOutStream &, const char *)
    DERIVE_FUNCTION_ARGS2(writeState, writeState2, SerializeOutStream &, const char *, StateTransmissionFlags)
    DERIVE_FUNCTION_ARGS(readState, SerializeInStream &, const char *)
    DERIVE_FUNCTION_ARGS2(readState, readState2, SerializeInStream &, const char *, StateTransmissionFlags)

    template <typename C>
    decltype(auto) resetOperation(C &c)
    {
        if constexpr (has_typename_ResetOperation_v<C>)
            return typename C::ResetOperation { c };
        else
            return c;
    }

    template <typename T, typename... Args>
    void read(SerializeInStream &in, T &t, const char *name, Args &&... args)
    {
        if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            in.format().beginPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            in.readUnformatted(t);
            in.format().endPrimitive(in, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (has_function_readState_v<T>) {
            t.readState(in, name);
        } else if constexpr (has_function_readState2_v<T>) {
            t.readState(in, name, StateTransmissionFlags_DontApplyMap);
        } else if constexpr (is_instance_v<T, std::unique_ptr>) {
            read(in, *t, name);
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

    template <typename C, typename Config, typename Op, typename... Args>
    void readContainerOp(SerializeInStream &in, C &container, const char *name, Op &op, Args &&... args)
    {
        using T = typename C::value_type;

        in.format().beginExtended(in, name, 1);
        uint32_t size;
        read(in, size, "size");
        in.format().beginCompound(in, name);

        if constexpr (!container_traits<C>::is_fixed_size) {
            TupleUnpacker::invoke(&Config::template clear<decltype(op)>, op, size);

            while (size--) {
                TupleUnpacker::invoke(&Config::template readItem<C, decltype(op)>, in, container, physical(container).end(), op, std::forward<Args>(args)...);
            }
        } else {
            assert(container.size() == size);
            for (T &t : container) {
                read(in, t, "Item");
            }
        }

        in.format().endCompound(in, name);
    }

    template <typename C, typename Config = typename has_typename_Config<C>::template type<DefaultCreator<typename C::value_type>>, typename... Args>
    void readContainer(SerializeInStream &in, C &container, const char *name, Args &&... args)
    {
        decltype(auto) op = resetOperation(container);
        readContainerOp<C, Config>(in, container, name, op, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    void write(SerializeOutStream &out, const T &t, const char *name, Args &&... args)
    {
        if constexpr (PrimitiveTypesContain_v<T> || std::is_enum_v<T>) {
            out.format().beginPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            out.writeUnformatted(t);
            out.format().endPrimitive(out, name, PrimitiveTypeIndex_v<T>);
            //mLog.log(t);
        } else if constexpr (has_function_writeState_v<T> || has_function_writeState2_v<T>) {
            t.writeState(out, name);
        } else if constexpr (is_instance_v<T, std::unique_ptr>) {
            write(out, *t, name);
        } else if constexpr (is_iterable_v<T>) {
            writeContainer(out, t, name, std::forward<Args>(args)...);
        } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
            write(out, TupleUnpacker::toTuple(t), name);
        } else {
            static_assert(dependent_bool<T, false>::value, "Invalid Type");
        }
    }

    template <typename T>
    void write(SerializeOutStream &out, const T &t)
    {
        write(out, t, nullptr);
    }

    template <typename... Ty>
    void write(SerializeOutStream &out, const std::tuple<Ty...> &t, const char *name = nullptr)
    {
        out.format().beginCompound(out, name);
        TupleUnpacker::forEach(t, [&](const auto &e) { write(out, e, "Element"); });
        out.format().endCompound(out, name);
    }

    template <typename C, typename Config = typename has_typename_Config<C>::template type<DefaultCreator<typename C::value_type>>, typename... Args>
    void writeContainer(SerializeOutStream &out, const C &container, const char *name, Args &&... args)
    {
        out.format().beginExtended(out, name, 1);
        write<uint32_t>(out, container.size(), "size");
        out.format().beginCompound(out, name);
        for (const auto &t : container) {
            TupleUnpacker::invoke(&Config::writeItem, out, t, std::forward<Args>(args)...);
        }
        out.format().endCompound(out, name);
    }

    template <typename T>
    inline SerializeInStream &SerializeInStream::operator>>(T &t)
    {
        read(*this, t);
        return *this;
    }

    template <typename T>
    inline SerializeOutStream &SerializeOutStream::operator<<(const T &t)
    {
        write(*this, t);
        return *this;
    }

    inline SerializeOutStream &SerializeOutStream::operator<<(const char *s)
    {
        write(*this, std::string { s });
        return *this;
    }

}
}