#pragma once

#include "../fixed_string.h"
#include "algorithm.h"

namespace Engine {
namespace Execution {

    template <fixed_string Name>
    struct Variable_t {
        template <typename Rec, typename T>
        struct state : base_state<Rec> {

            void start()
            {
                mRec.set_value();
            }
            
            T mData;
        };

        template <typename T>
        friend auto tag_invoke(Variable_t, T &&initialValue)
        {
            return make_sender<void>(
                [initialValue { forward_capture(std::forward<T>(initialValue)) }]<typename Rec>(Rec &&rec) mutable {
                    return state<Rec, T> { std::forward<Rec>(rec), std::forward<T>(initialValue) };
                });
        }

        template <typename T>
        requires tag_invocable<Variable_t, T>
        auto operator()(T &&initialValue) const
            noexcept(is_nothrow_tag_invocable_v<Variable_t, T>)
                -> tag_invoke_result_t<Variable_t, T>
        {
            return tag_invoke(*this, std::forward<T>(initialValue));
        }
    };

    template <fixed_string Name>
    inline constexpr Variable_t<Name> Variable;

    template <fixed_string Name>
    struct resolve_t {

        template <typename T>
        auto operator()(T &&t) const
            noexcept(is_nothrow_tag_invocable_v<resolve_t, T>)
                -> tag_invoke_result_t<resolve_t, T>
        {
            return tag_invoke(*this, std::forward<T>(t));
        }
    };

    template <fixed_string Name>
    inline constexpr resolve_t<Name> resolve;

    template <fixed_string Name>
    struct read_t {
        template <typename Rec>
        struct state : base_state<Rec>{

            void start()
            {
                mRec.set_value(resolve<Name>(mRec));
            }
        };

        friend auto tag_invoke(const read_t &)
        {
            return make_sender<void, int>(
                []<typename Rec>(Rec &&rec) mutable {
                    return state<Rec> { std::forward<Rec>(rec) };
                });
        }

        auto operator()() const
            noexcept(is_nothrow_tag_invocable_v<read_t>)
                -> tag_invoke_result_t<read_t>
        {
            return tag_invoke(*this);
        }
    };

    template <fixed_string Name>
    inline constexpr read_t<Name> read;
}
}