#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/fixed_string.h"
#include "behaviorerror.h"

namespace Engine {
namespace Execution {

    struct resolve_var_d_t {
        template <typename V, typename O>
        requires(!is_tag_invocable_v<resolve_var_d_t, V &, std::string_view, O &>) auto operator()(V &v, std::string_view name, O &out) const
        {
            return false;
        }

        template <typename V, typename O>
        requires(is_tag_invocable_v<resolve_var_d_t, V &, std::string_view, O &>) auto operator()(V &v, std::string_view name, O &out) const
            noexcept(is_nothrow_tag_invocable_v<resolve_var_d_t, V &, std::string_view, O &>)
                -> tag_invoke_result_t<resolve_var_d_t, V &, std::string_view, O &>
        {
            return tag_invoke(*this, v, name, out);
        }
    };

    inline constexpr resolve_var_d_t resolve_var_d;

    struct store_var_d_t {
        template <typename V, typename T>
        requires(!is_tag_invocable_v<store_var_d_t, V &, std::string_view, T>) auto operator()(V &v, std::string_view name, T &&value) const
        {
            return false;
        }

        template <typename V, typename T>
        requires(is_tag_invocable_v<store_var_d_t, V &, std::string_view, T>) auto operator()(V &v, std::string_view name, T &&value) const
            noexcept(is_nothrow_tag_invocable_v<store_var_d_t, V &, std::string_view, T>)
                -> tag_invoke_result_t<store_var_d_t, V &, std::string_view, T>
        {
            return tag_invoke(*this, v, name, std::forward<T>(value));
        }
    };

    inline constexpr store_var_d_t store_var_d;

    template <fixed_string Name>
    struct resolve_var_t {

        template <typename V, typename O>
        requires(!is_tag_invocable_v<resolve_var_t, V, O &>) decltype(auto) operator()(V &&v, O &out) const
        {
            return resolve_var_d(std::forward<V>(v), Name, out);
        }

        template <typename V, typename O>
        requires(is_tag_invocable_v<resolve_var_t, V, O &>) auto operator()(V &&v, O &out) const
            noexcept(is_nothrow_tag_invocable_v<resolve_var_t, V, O &>)
                -> tag_invoke_result_t<resolve_var_t, V, O &>
        {
            return tag_invoke(*this, std::forward<V>(v), out);
        }
    };

    template <fixed_string Name>
    inline constexpr resolve_var_t<Name> resolve_var;

    template <fixed_string Name>
    struct store_var_t {

        template <typename V, typename T>
        requires(!is_tag_invocable_v<store_var_t, V, T>) decltype(auto) operator()(V &&v, T &&value) const
        {
            return store_var_d(std::forward<V>(v), Name, std::forward<T>(value));
        }

        template <typename V, typename T>
        requires(is_tag_invocable_v<store_var_t, V, T>) auto operator()(V &&v, T &&value) const
            noexcept(is_nothrow_tag_invocable_v<store_var_t, V, T>)
                -> tag_invoke_result_t<store_var_t, V, T>
        {
            return tag_invoke(*this, std::forward<V>(v), std::forward<T>(value));
        }
    };

    template <fixed_string Name>
    inline constexpr store_var_t<Name> store_var;

    struct read_var_t {
        template <fixed_string Name, typename T, typename Rec>
        struct state : base_state<Rec> {
            void start()
            {
                std::conditional_t<std::is_reference_v<T>, OutRef<std::remove_reference_t<T>>, T> result;
                if (resolve_var<Name>(this->mRec, result)) {
                    this->mRec.set_value(std::forward<T>(result));
                } else {
                    std::string errorMsg = "Variable \""s + Name.c_str() + "\" not found.";
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
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
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
    inline constexpr read_var_t::Inner<Name, T> read_var;

    struct write_var_t {
        template <fixed_string Name, typename Rec>
        struct receiver : algorithm_receiver<Rec> {
            template <typename V>
            void set_value(V &&value)
            {
                if (store_var<Name>(this->mRec, std::forward<V>(value))) {
                    this->mRec.set_value();
                } else {
                    std::string errorMsg = "Variable \""s + Name.c_str() + "\" not found.";
                    this->mRec.set_error(BehaviorError {
                        GenericResult::UNKNOWN_ERROR,
                        errorMsg });
                }
            }
        };

        template <fixed_string Name, typename Sender>
        struct sender : algorithm_sender<Sender> {

            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return algorithm_state<Sender, receiver<Name, Rec>> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec) };
            }
        };

        template <fixed_string Name>
        struct Inner {
            template <typename Sender>
            friend auto tag_invoke(const Inner &, Sender &&inner)
            {
                return sender<Name, Sender> { { {}, std::forward<Sender>(inner) } };
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

    struct Variable_t {

        template <fixed_string Name, typename Rec, typename Sender, typename T>
        struct state;

        template <fixed_string Name, typename Rec, typename Sender, typename T>
        struct receiver : algorithm_receiver<Rec> {

            template <typename O>
            friend bool tag_invoke(resolve_var_t<Name>, receiver &rec, O &out)
            {
                out = rec.mState->mData;
                return true;
            }

            template <typename V>
            friend bool tag_invoke(store_var_t<Name>, receiver &rec, V &&value)
            {
                rec.mState->mData = std::forward<V>(value);
                return true;
            }

            template <typename O>
            friend auto tag_invoke(resolve_var_d_t, receiver &rec, std::string_view name, O &out)
            {
                if (name == Name) {
                    out = rec.mState->mData;
                    return true;
                } else {
                    return resolve_var_d(rec.mRec, name, out);
                }
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

            T mInitialValue;
        };

        template <fixed_string Name>
        struct Inner {
            template <typename Sender, typename T>
            friend auto tag_invoke(Inner, Sender &&inner, T &&initialValue)
            {
                return sender<Name, Sender, T> { { {}, std::forward<Sender>(inner) }, std::forward<T>(initialValue) };
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

}
}