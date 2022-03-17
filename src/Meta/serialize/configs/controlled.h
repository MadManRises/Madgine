#pragma once

#include "../operations.h"

namespace Engine {
namespace Serialize {

    struct CreatorCategory;

    template <typename Cmp>
    struct ControlledConfig {

        using Category = CreatorCategory;

        static const constexpr bool controlled = true;

        template <typename C>
        static void writeItem(FormattedSerializeStream &out, const typename container_traits<C>::value_type &t)
        {
            out.beginExtendedWrite("Item", 1);
            write(out, comparator_traits<Cmp>::to_cmp_type(t), "key");
            write(out, t, "Item");
        }

        template <typename Op>
        static StreamResult readItem(FormattedSerializeStream &in, Op &op, typename container_traits<Op>::iterator &it)
        {
            /*if (it != physical(op).end())
                return STREAM_ERROR(in, StreamState::UNKNOWN_ERROR, "Reading currently only supported at end()");*/

            STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Item", 1));
            MakeOwning_t<typename comparator_traits<Cmp>::type> key;
            STREAM_PROPAGATE_ERROR(read(in, key, "key"));
            it = std::ranges::find(physical(op), key, &comparator_traits<Cmp>::to_cmp_type);
            if (it == physical(op).end())
                return STREAM_UNKNOWN_ERROR(in) << "Missing item of name '" << key << "' in controlled container";

            return read(in, *it, "Item");
        }

        template <typename Op>
        static void clear(Op &op)
        {
            /*if (op.size() < expected) //TODO: ?
                std::terminate();*/
        }
    };

}
}