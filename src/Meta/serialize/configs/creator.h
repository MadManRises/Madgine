#pragma once

#include "Generic/callerhierarchy.h"

#include "Generic/makeowning.h"

#include "Generic/functor.h"

namespace Engine {
namespace Serialize {

    template <typename KeyCreator, typename ValueCreator>
    struct KeyValueCreator {

        using Category = CreatorCategory;

        static const constexpr bool controlled = false;

        template <typename P>
        using ArgsTuple = std::tuple<std::piecewise_construct_t, typename KeyCreator::template ArgsTuple<typename P::first_type>, typename ValueCreator::template ArgsTuple<typename P::second_type>>;

        template <typename C>
        static void writeItem(FormattedSerializeStream &out, const typename container_traits<C>::value_type &t)
        {
            out.beginCompoundWrite("Item");
            writeCreationData<typename container_traits<C>::value_type>(out, t);
            write<typename container_traits<C>::value_type::first_type>(out, t.first, "Key");
            write<typename container_traits<C>::value_type::second_type>(out, t.second, "Value");
            out.endCompoundWrite("Item");
        }

        template <typename Op>
        static StreamResult readItem(FormattedSerializeStream &in, Op &op, typename container_traits<Op>::emplace_return &it, const typename container_traits<Op>::const_iterator &where)
        {
            STREAM_PROPAGATE_ERROR(in.beginCompoundRead(nullptr));
            ArgsTuple<typename container_traits<Op>::value_type> tuple;
            STREAM_PROPAGATE_ERROR(readCreationData<typename container_traits<Op>::value_type>(in, tuple));
            it = TupleUnpacker::invokeExpand(LIFT(container_traits<Op>::emplace), op, where, std::move(tuple));
            assert(container_traits<Op>::was_emplace_successful(it));
            STREAM_PROPAGATE_ERROR(read<typename container_traits<Op>::value_type::first_type>(in, it.first->first, "Key"));
            STREAM_PROPAGATE_ERROR(read<typename container_traits<Op>::value_type::second_type>(in, it.first->second, "Value"));
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
        static void writeItem(FormattedSerializeStream &out, const typename container_traits<C>::value_type &t)
        {
            writeCreationData<typename container_traits<C>::value_type>(out, t);
            write<typename container_traits<C>::value_type>(out, t, "Item");
        }

        template <typename Op>
        static StreamResult readItem(FormattedSerializeStream &in, Op &op, typename container_traits<Op>::emplace_return &it, const typename container_traits<Op>::const_iterator &where)
        {
            ArgsTuple<typename container_traits<Op>::value_type> tuple;
            STREAM_PROPAGATE_ERROR(readCreationData<typename container_traits<Op>::value_type>(in, tuple));
            it = TupleUnpacker::invokeExpand(LIFT(container_traits<Op>::emplace), op, where, std::move(tuple));
            STREAM_PROPAGATE_ERROR(read(in, *it, "Item"));
            assert(container_traits<Op>::was_emplace_successful(it));
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
            static StreamResult readItem(FormattedSerializeStream &in, Op &op, typename container_traits<Op>::emplace_return &it, const typename container_traits<Op>::const_iterator &where, const CallerHierarchyBasePtr &hierarchy)
            {
                ArgsTuple tuple;
                STREAM_PROPAGATE_ERROR(readCreationData(in, tuple));
                if constexpr (std::is_const_v<typename container_traits<Op>::value_type>) {

                    std::remove_const_t<typename container_traits<Op>::value_type> temp = TupleUnpacker::constructFromTuple<std::remove_const_t<typename container_traits<Op>::value_type>>(std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, temp, nullptr, hierarchy));
                    it = container_traits<Op>::emplace(op, where, std::move(temp));
                } else {
                    it = TupleUnpacker::invokeExpand(LIFT(container_traits<Op>::emplace), op, where, std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, *it, nullptr, hierarchy));
                }
                assert(container_traits<Op>::was_emplace_successful(it));
                return {};
            }

            template <typename Arg>
            static void writeCreationData(FormattedSerializeStream &out, const Arg &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                WriteFunctor {}(out, arg);
            }

            template <typename C>
            static void writeItem(FormattedSerializeStream &out, const typename container_traits<C>::value_type &arg, const CallerHierarchyBasePtr &hierarchy)
            {
                writeCreationData<typename container_traits<C>::value_type>(out, arg, hierarchy);
                write<typename container_traits<C>::value_type>(out, arg, "Item", hierarchy);
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

            using ArgsTuple = std::tuple<MakeOwning_t<std::remove_const_t<std::remove_reference_t<_Ty>>>...>;

            static StreamResult readCreationData(FormattedSerializeStream &in, ArgsTuple &result, const CallerHierarchy<T *> &parent)
            {
                assert(parent.mData);
                if constexpr (std::same_as<R, void>) {
                    TupleUnpacker::invokeExpand(reader, parent.mData, in, result);
                    return {};
                } else {
                    return TupleUnpacker::invokeExpand(reader, parent.mData, in, result);
                }
            }

            template <typename Op>
            static StreamResult readItem(FormattedSerializeStream &in, Op &op, typename container_traits<Op>::emplace_return &it, const typename container_traits<Op>::const_iterator &where, const CallerHierarchy<T *> &parent)
            {
                ArgsTuple tuple;
                STREAM_PROPAGATE_ERROR(readCreationData(in, tuple, parent));
                if constexpr (std::is_const_v<typename container_traits<Op>::value_type>) {
                    std::remove_const_t<typename container_traits<Op>::value_type> temp = TupleUnpacker::constructFromTuple<std::remove_const_t<typename container_traits<Op>::value_type>>(std::move(*tuple));
                    STREAM_PROPAGATE_ERROR(read(in, temp, nullptr, CallerHierarchyPtr { parent }));
                    it = container_traits<Op>::emplace(op, where, std::move(temp));
                } else {
                    it = TupleUnpacker::invokeFlatten(LIFT(container_traits<Op>::emplace), op, where, std::move(tuple));
                    STREAM_PROPAGATE_ERROR(read(in, *it, nullptr, CallerHierarchyPtr { parent }));
                }
                assert(container_traits<Op>::was_emplace_successful(it));
                return {};
            }

            template <typename Arg>
            static void writeCreationData(FormattedSerializeStream &out, const Arg &arg, const CallerHierarchy<const T *> &parent)
            {
                assert(parent.mData);
                WriteFunctor {}(parent.mData, out, arg);
            }

            template <typename C>
            static void writeItem(FormattedSerializeStream &out, const typename container_traits<C>::value_type &arg, const CallerHierarchy<const T *> &parent)
            {
                writeCreationData<typename container_traits<C>::value_type>(out, arg, parent);
                write<typename container_traits<C>::value_type>(out, arg, "Item", CallerHierarchyPtr { parent });
            }

            template <typename Op>
            static void clear(Op &op, const CallerHierarchy<T *> &parent)
            {
                ClearFunctor {}(parent.mData, op);
            }
        };

    }

    template <auto reader, auto writer, auto clear = nullptr>
    using ParentCreator = typename FunctionCapture<__serialize_impl__::_ParentCreator, reader, MemberFunctor<writer>, std::conditional_t<std::is_same_v<decltype(clear), std::nullptr_t>, __serialize_impl__::DefaultClear, UnpackingMemberFunctor<clear>>>::type;

    template <auto reader, auto writer, auto clear = nullptr>
    using CustomCreator = typename FunctionCapture<__serialize_impl__::_CustomCreator, reader, Functor<writer>, std::conditional_t<std::is_same_v<decltype(clear), std::nullptr_t>, __serialize_impl__::DefaultClear, Functor<clear>>>::type;
}
}
