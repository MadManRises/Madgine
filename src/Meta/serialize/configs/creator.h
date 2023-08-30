#pragma once

#include "Generic/callerhierarchy.h"

#include "Generic/makeowning.h"

#include "Generic/customfunctors.h"

#include "Generic/container/emplace.h"

#include "../streams/formattedserializestream.h"

namespace Engine {
namespace Serialize {

    template <typename KeyCreator, typename ValueCreator>
    struct KeyValueCreator {

        using Category = CreatorCategory;

        static const constexpr bool controlled = false;

        template <typename P>
        using ArgsTuple = std::tuple<std::piecewise_construct_t, typename KeyCreator::template ArgsTuple<typename P::first_type>, typename ValueCreator::template ArgsTuple<typename P::second_type>>;

        template <typename C>
        static void writeItem(FormattedSerializeStream &out, const std::ranges::range_value_t<C> &t)
        {
            out.beginCompoundWrite("Item");
            writeCreationData<std::ranges::range_value_t<C>>(out, t);
            write<typename std::ranges::range_value_t<C>::first_type>(out, t.first, "Key");
            write<typename std::ranges::range_value_t<C>::second_type>(out, t.second, "Value");
            out.endCompoundWrite("Item");
        }

        template <typename Op>
        static StreamResult readItem(FormattedSerializeStream &in, Op &op, std::ranges::iterator_t<Op> &it, const std::ranges::const_iterator_t<Op> &where)
        {
            STREAM_PROPAGATE_ERROR(in.beginCompoundRead(nullptr));
            ArgsTuple<std::ranges::range_value_t<Op>> tuple;
            STREAM_PROPAGATE_ERROR(readCreationData<std::ranges::range_value_t<Op>>(in, tuple));
            bool success;
            it = TupleUnpacker::invokeExpand(emplace, success, op, where, std::move(tuple));
            assert(success);
            STREAM_PROPAGATE_ERROR(read<typename std::ranges::range_value_t<Op>::first_type>(in, it->first, "Key"));
            STREAM_PROPAGATE_ERROR(read<typename std::ranges::range_value_t<Op>::second_type>(in, it->second, "Value"));
            return in.endCompoundRead(nullptr);
        }

        template <typename P>
        static void writeCreationData(FormattedSerializeStream &out, const P &t)
        {
            KeyCreator::template writeCreationData<typename P::first_type>(out, t.first, "Key");
            ValueCreator::template writeCreationData<typename P::second_type>(out, t.second, "Value");
        }

        template <typename P>
        static StreamResult readCreationData(FormattedSerializeStream &in, ArgsTuple<P> &tuple)
        {
            STREAM_PROPAGATE_ERROR(KeyCreator::template readCreationData<typename P::first_type>(in, std::get<1>(tuple)));
            return ValueCreator::template readCreationData<typename P::second_type>(in, std::get<2>(tuple));
        }

        template <typename Op>
        static void clear(Op &op)
        {
            op.clear();
        }
    };

    struct DefaultCreator {
        using Category = CreatorCategory;

        template <typename T>
        using ArgsTuple = std::conditional_t<std::is_const_v<T>, std::tuple<std::remove_const_t<T>>, std::tuple<>>;

        static const constexpr bool controlled = false;

        template <typename T>
        static StreamResult readCreationData(FormattedSerializeStream &in, ArgsTuple<T> &result)
        {
            if constexpr (std::is_const_v<T>) {
                return read<std::remove_const_t<T>>(in, std::get<0>(result), nullptr);
            } else {
                return {};
            }
        }

        template <typename T>
        static void writeCreationData(FormattedSerializeStream &out, const T &t, const char *name = "Item")
        {
            if constexpr (std::is_const_v<T>) {
                write<std::remove_const_t<T>>(out, t, name);
            }
        }

        template <typename C>
        static void writeItem(FormattedSerializeStream &out, const std::ranges::range_value_t<C> &t)
        {
            using T = std::ranges::range_value_t<C>;
            writeCreationData<T>(out, t);
            if constexpr (!std::is_const_v<T>) {
                write<T>(out, t, "Item");
            }
        }

        template <typename Op>
        static StreamResult readItem(FormattedSerializeStream &in, Op &op, std::ranges::iterator_t<Op> &it, const std::ranges::const_iterator_t<Op> &where)
        {
            using T = std::remove_reference_t<std::ranges::range_reference_t<Op>>;
            ArgsTuple<T> tuple;
            STREAM_PROPAGATE_ERROR(readCreationData<T>(in, tuple));
            bool success;
            it = TupleUnpacker::invokeExpand(emplace, success, op, where, std::move(tuple));
            if constexpr (!std::is_const_v<T>) {
                STREAM_PROPAGATE_ERROR(read(in, *it, "Item"));
            }
            assert(success);
            return {};
        }

        template <typename Op>
        static void clear(Op &op)
        {
            op.clear();
        }
    };

    namespace __serialize_impl__ {

        struct DefaultClear {
            template <typename T, typename Op>
            void operator()(T &&t, Op &op)
            {
                op.clear();
            }
            template <typename Op>
            void operator()(Op &op)
            {
                op.clear();
            }
        };

        template <auto reader, typename WriteFunctor, typename ClearFunctor, OneOf<void, StreamResult> R, typename T, typename Stream, typename... _Ty>
        struct _CustomCreator {

            using Category = CreatorCategory;

            static const constexpr bool controlled = false;

            using ArgsTuple = std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...>;

            static StreamResult readCreationData(FormattedSerializeStream &in, ArgsTuple &result)
            {
                if constexpr (std::same_as<R, void>) {
                    TupleUnpacker::invokeExpand(reader, in, result);
                    return {};
                } else {
                    return TupleUnpacker::invokeExpand(reader, in, result);
                }
            }

            template <typename Op>
            static StreamResult readItem(FormattedSerializeStream &in, Op &op, std::ranges::iterator_t<Op> &it, const std::ranges::const_iterator_t<Op> &where, const CallerHierarchyBasePtr &hierarchy)
            {
                ArgsTuple tuple;
                bool success;
                STREAM_PROPAGATE_ERROR(readCreationData(in, tuple));
                if constexpr (std::is_const_v<std::ranges::range_value_t<Op>>) {

                    std::remove_const_t<std::ranges::range_value_t<Op>> temp = TupleUnpacker::constructFromTuple<std::remove_const_t<std::ranges::range_value_t<Op>>>(std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, temp, nullptr, hierarchy));
                    it = emplace(success, op, where, std::move(temp));
                } else {
                    it = TupleUnpacker::invokeExpand(emplace, success, op, where, std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, *it, nullptr, hierarchy));
                }
                assert(success);
                return {};
            }

            template <typename Arg>
            static const char *writeCreationData(FormattedSerializeStream &out, const Arg &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                return WriteFunctor {}(out, arg);
            }

            template <typename C>
            static void writeItem(FormattedSerializeStream &out, const std::ranges::range_value_t<C> &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                const char *name = writeCreationData<std::ranges::range_value_t<C>>(out, arg, hierarchy);
                write<std::ranges::range_value_t<C>>(out, arg, name, hierarchy);
            }

            template <typename Op>
            static void clear(Op &op)
            {
                ClearFunctor {}(op);
            }
        };

        template <auto reader, typename WriteFunctor, typename ClearFunctor, OneOf<void, StreamResult> R, typename T, typename Stream, typename... _Ty>
        struct _ParentCreator {

            using Category = CreatorCategory;

            static const constexpr bool controlled = false;

            using ArgsTuple = std::tuple<MakeOwning_t<_Ty>...>;

            static StreamResult readCreationData(FormattedSerializeStream &in, ArgsTuple &result, const CallerHierarchyBasePtr &hierarchy)
            {
                T *parent = hierarchy;
                assert(parent);
                if constexpr (std::same_as<R, void>) {
                    TupleUnpacker::invokeExpand(reader, parent, in, result);
                    return {};
                } else {
                    return TupleUnpacker::invokeExpand(reader, parent, in, result);
                }
            }

            template <typename Op>
            static StreamResult readItem(FormattedSerializeStream &in, Op &op, std::ranges::iterator_t<Op> &it, const std::ranges::const_iterator_t<Op> &where, const CallerHierarchyBasePtr &hierarchy)
            {
                ArgsTuple tuple;
                bool success;
                STREAM_PROPAGATE_ERROR(readCreationData(in, tuple, hierarchy));
                if constexpr (std::is_const_v<std::ranges::range_value_t<Op>>) {
                    std::remove_const_t<std::ranges::range_value_t<Op>> temp = TupleUnpacker::constructFromTuple<std::remove_const_t<std::ranges::range_value_t<Op>>>(std::move(*tuple));
                    STREAM_PROPAGATE_ERROR(read(in, temp, nullptr, hierarchy));
                    it = emplace(success, op, where, std::move(temp));
                } else {
                    it = TupleUnpacker::invokeFlatten(emplace, success, op, where, std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, *it, nullptr, hierarchy));
                }
                assert(success);
                return {};
            }

            template <typename Arg>
            static const char *writeCreationData(FormattedSerializeStream &out, const Arg &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                const T *parent = hierarchy;
                assert(parent);
                return WriteFunctor {}(parent, out, arg);
            }

            template <typename C>
            static void writeItem(FormattedSerializeStream &out, const std::ranges::range_value_t<C> &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                const char *name = writeCreationData<std::ranges::range_value_t<C>>(out, arg, hierarchy);
                write<std::ranges::range_value_t<C>>(out, arg, name, hierarchy);
            }

            template <typename Op>
            static void clear(Op &op, const CallerHierarchyBasePtr &hierarchy)
            {
                T *parent = hierarchy;
                ClearFunctor {}(parent, op);
            }
        };

    }

    template <auto reader, auto writer, auto clear = nullptr>
    using ParentCreator = typename FunctionCapture<__serialize_impl__::_ParentCreator, reader, MemberFunctor<writer>, std::conditional_t<std::same_as<decltype(clear), std::nullptr_t>, __serialize_impl__::DefaultClear, UnpackingMemberFunctor<clear>>>::type;

    template <auto reader, auto writer, auto clear = nullptr>
    using CustomCreator = typename FunctionCapture<__serialize_impl__::_CustomCreator, reader, Functor<writer>, std::conditional_t<std::same_as<decltype(clear), std::nullptr_t>, __serialize_impl__::DefaultClear, Functor<clear>>>::type;

    template <typename... Configs>
    using CreatorSelector = ConfigSelectorDefault<CreatorCategory, DefaultCreator, Configs...>;
}
}
