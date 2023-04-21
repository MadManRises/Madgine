#pragma once

#include "../fixed_string.h"
#include "concepts.h"

namespace Engine {
namespace Execution {

    template <fixed_string Name>
    struct resolve_var_t;

    struct Variable_t {

        template <fixed_string Name, typename Rec, typename Sender, typename T>
        struct state;

        template <fixed_string Name, typename Rec, typename Sender, typename T>
        struct receiver : algorithm_receiver<Rec> {

            friend T &tag_invoke(resolve_var_t<Name>, receiver &rec)
            {
                return rec.mState->mData;
            }

            state<Name, Rec, Sender, T> *mState;
        };

        template <fixed_string Name, typename Rec, typename Sender, typename T>
        struct state : algorithm_state<Sender, receiver<Name, Rec, Sender, T>> {
            state(Rec &&rec, Sender &&sender, T &&initialValue)
                : algorithm_state<Sender, receiver<Name, Rec, Sender, T>> { std::forward<Sender>(sender), std::forward<Rec>(rec), this }
                , mData(std::forward<T>(initialValue))
            {
            }

            T mData;
        };

        template <fixed_string Name, typename Sender, typename T>
        struct sender : algorithm_sender<Sender> {

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Name, Rec, Sender, T> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender), std::forward<T>(sender.mInitialValue) };
            }

            Sender mSender;
            T mInitialValue;
        };

        template <fixed_string Name>
        struct Inner {
            template <typename Sender, typename T>
            friend auto tag_invoke(Inner, Sender &&inner, T &&initialValue)
            {
                return sender<Name, Sender, T> { {}, std::forward<Sender>(inner), std::forward<T>(initialValue) };
            }

            template <typename Sender, typename T>
            requires tag_invocable<Inner, Sender, T>
            auto operator()(Sender &&sender, T &&initialValue) const
                noexcept(is_nothrow_tag_invocable_v<Inner, Sender, T>)
                    -> tag_invoke_result_t<Inner, Sender, T>
            {
                return tag_invoke(*this, std::forward<Sender>(sender), std::forward<T>(initialValue));
            }

            template <typename T>
            auto operator()(T &&t) const
            {
                return pipable_from_right(*this, std::forward<T>(t));
            }
        };
    };

    template <fixed_string Name>
    inline constexpr Variable_t::Inner<Name> Variable;

    template <fixed_string Name>
    struct resolve_var_t {

        template <typename T>
        auto operator()(T &&t) const
            noexcept(is_nothrow_tag_invocable_v<resolve_var_t, T>)
                -> tag_invoke_result_t<resolve_var_t, T>
        {
            return tag_invoke(*this, std::forward<T>(t));
        }
    };

    template <fixed_string Name>
    inline constexpr resolve_var_t<Name> resolve_var;

    struct read_var_t {
        template <fixed_string Name, typename Rec>
        struct state : base_state<Rec> {
            void start()
            {
                this->mRec.set_value(resolve_var<Name>(this->mRec));
            }
        };

        template <typename T, fixed_string Name>
        struct sender {
            using result_type = void;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<T>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Name, Rec> { std::forward<Rec>(rec) };
            }
        };

        template <typename T, fixed_string Name>
        struct Inner {
            friend auto tag_invoke(const Inner<T, Name> &)
            {
                return sender<T, Name> {};
            }

            auto operator()() const
                noexcept(is_nothrow_tag_invocable_v<Inner<T, Name>>)
                    -> tag_invoke_result_t<Inner<T, Name>>
            {
                return tag_invoke(*this);
            }
        };
    };

    template <typename T, fixed_string Name>
    inline constexpr read_var_t::Inner<T, Name> read_var;

    struct write_var_t {
        template <fixed_string Name, typename Rec>
        struct receiver : algorithm_receiver<Rec> {
            template <typename V>
            void set_value(V &&value)
            {
                resolve_var<Name>(this->mRec) = std::forward<V>(value);
                this->mRec.set_value();
            }
        };

        template <typename Sender, fixed_string Name>
        struct sender : algorithm_sender<Sender> {

            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return algorithm_state<Sender, receiver<Name, Rec>> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec) };
            }

            Sender mSender;
        };

        template <fixed_string Name>
        struct Inner {
            template <typename Sender>
            friend auto tag_invoke(const Inner &, Sender &&inner)
            {
                return sender<Sender, Name> { {}, std::forward<Sender>(inner) };
            }

            template <typename Sender>
            auto operator()(Sender &&sender) const
                noexcept(is_nothrow_tag_invocable_v<Inner, Sender>)
                    -> tag_invoke_result_t<Inner, Sender>
            {
                return tag_invoke(*this, std::forward<Sender>(sender));
            }

            auto operator()() const
            {
                return pipable_from_right(*this);
            }
        };
    };

    template <fixed_string Name>
    inline constexpr write_var_t::Inner<Name> write_var;
}
}