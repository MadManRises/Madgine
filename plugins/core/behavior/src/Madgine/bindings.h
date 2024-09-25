#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/fixed_string.h"

namespace Engine {

    struct get_binding_d_t {
        template <typename V, typename O>
        requires(!is_tag_invocable_v<get_binding_d_t, V &, std::string_view, O &>) auto operator()(V &v, std::string_view name, O &out) const
        {
            return false;
        }

        template <typename V, typename O>
        requires(is_tag_invocable_v<get_binding_d_t, V &, std::string_view, O &>) auto operator()(V &v, std::string_view name, O &out) const
            noexcept(is_nothrow_tag_invocable_v<get_binding_d_t, V &, std::string_view, O &>)
                -> tag_invoke_result_t<get_binding_d_t, V &, std::string_view, O &>
        {
            return tag_invoke(*this, v, name, out);
        }
    };

    inline constexpr get_binding_d_t get_binding_d;
        
    template <fixed_string Name>
    struct get_binding_t {

        template <typename V, typename O>
        requires(!is_tag_invocable_v<get_binding_t, V, O &>) decltype(auto) operator()(V &&v, O &out) const
        {
            return get_binding_d(std::forward<V>(v), Name, out);
        }

        template <typename V, typename O>
        requires(is_tag_invocable_v<get_binding_t, V, O &>) auto operator()(V &&v, O &out) const
            noexcept(is_nothrow_tag_invocable_v<get_binding_t, V, O &>)
                -> tag_invoke_result_t<get_binding_t, V, O &>
        {
            return tag_invoke(*this, std::forward<V>(v), out);
        }
    };

    template <fixed_string Name>
    inline constexpr get_binding_t<Name> get_binding;

    struct get_binding_sender_t {
        template <fixed_string Name, typename T, typename Rec>
        struct state : Execution::base_state<Rec> {
            void start()
            {
                std::conditional_t<std::is_reference_v<T>, OutRef<std::remove_reference_t<T>>, T> result;
                if (get_binding<Name>(this->mRec, result)) {
                    this->mRec.set_value(std::forward<T>(result));
                } else {
                    std::string errorMsg = "Binding \""s + Name.c_str() + "\" not found.";
                    this->mRec.set_error(BehaviorError {
                        GenericResult::UNKNOWN_ERROR,
                        errorMsg });
                }
            }
        };

        template <fixed_string Name, typename T>
        struct sender {
            using is_sender = void;

            using result_type = BehaviorError;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<T>;

            template <typename Rec>
            friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
            {
                return state<Name, T, Rec> { std::forward<Rec>(rec) };
            }
        };

        template <fixed_string Name, typename T>
        struct Inner {
            friend auto tag_invoke(const Inner<Name, T> &)
            {
                return sender<Name, T> {};
            }

            auto operator()() const
                noexcept(is_nothrow_tag_invocable_v<Inner<Name, T>>)
                    -> tag_invoke_result_t<Inner<Name, T>>
            {
                return tag_invoke(*this);
            }
        };
    };

    template <fixed_string Name, typename T>
    inline constexpr get_binding_sender_t::Inner<Name, T> get_binding_sender;



}