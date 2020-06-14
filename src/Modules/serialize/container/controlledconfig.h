#pragma once

#include "../../generic/comparator_traits.h"
#include "physical.h"
#include "../../generic/fixstring.h"

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

        template <typename C, typename Op>
        static typename C::iterator readItem(SerializeInStream& in, C &c) {

            in.format().beginExtended(in, "Item", 1);
            FixString_t<typename comparator_traits<Cmp>::type> key;
            read(in, key, "key");
            typename C::iterator it = std::find_if(physical(c).begin(), physical(c).end(), [&](const auto &t) {
                return comparator_traits<Cmp>::to_cmp_type(t) == key;
            });
            if (it == physical(c).end())
                std::terminate();

            read(in, *it, "Item");     

            return it;
        }

        template <typename Op>
        static void clear(Op &op, uint32_t expected)
        {
            if (op.size() != expected)
                std::terminate();
        }

    };

}
}