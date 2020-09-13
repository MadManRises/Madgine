#pragma once

#include "../../generic/comparator_traits.h"
#include "physical.h"
#include "../../generic/makeowning.h"
#include "../streams/serializestream.h"
#include "../formatter.h"

namespace Engine {
namespace Serialize {

	template <typename Cmp>
    struct ControlledConfig : comparator_traits<Cmp>{
    
        static const constexpr bool controlled = true;
    
        static void writeItem(SerializeOutStream &out, const typename comparator_traits<Cmp>::item_type &t)
        {
            out.format().beginExtended(out, "Item", 1);
            write(out, comparator_traits<Cmp>::to_cmp_type(t), "key");
            write(out, t, "Item");
        }

        template <typename Op>
        static typename container_traits<Op>::iterator readItem(SerializeInStream& in, Op &op) {

            in.format().beginExtended(in, "Item", 1);
            MakeOwning_t<typename comparator_traits<Cmp>::type> key;
            read(in, key, "key");
            typename container_traits<Op>::iterator it = std::find_if(physical(op).begin(), physical(op).end(), [&](const auto &t) {
                return comparator_traits<Cmp>::to_cmp_type(t) == key;
            });
            if (it == physical(op).end())
                std::terminate();

            read(in, *it, "Item");     

            return it;
        }

        template <typename Op>
        static void clear(Op &op, uint32_t expected)
        {
            if (op.size() < expected) //TODO: ?
                std::terminate();
        }

    };

}
}