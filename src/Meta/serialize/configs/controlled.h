#pragma once

#include "../formatter.h"
#include "../streams/serializestream.h"
#include "Generic/comparator_traits.h"
#include "Generic/makeowning.h"
#include "../container/physical.h"

namespace Engine {
namespace Serialize {

    struct CreatorCategory;

    template <typename Cmp>
    struct ControlledConfig {

        using Category = CreatorCategory;

        static const constexpr bool controlled = true;

        static void writeItem(SerializeOutStream &out, const typename comparator_traits<Cmp>::item_type &t)
        {
            out.format().beginExtended(out, "Item", 1);
            write(out, comparator_traits<Cmp>::to_cmp_type(t), "key");
            write(out, t, "Item");
        }

        template <typename Op>
        static StreamResult readItem(SerializeInStream &in, Op &op, typename container_traits<Op>::iterator &it)
        {

            STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, "Item", 1));
            MakeOwning_t<typename comparator_traits<Cmp>::type> key;
            STREAM_PROPAGATE_ERROR(read(in, key, "key"));
            it = std::find_if(physical(op).begin(), physical(op).end(), [&](const auto &t) {
                return comparator_traits<Cmp>::to_cmp_type(t) == key;
            });
            if (it == physical(op).end())
                return STREAM_ERROR(in, StreamState::UNKNOWN_ERROR, "Reading currently only supported at end()");

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