#pragma once

#include "../tag_invoke.h"

namespace Engine {
namespace Execution {

    template <typename T>
    concept Sender = requires
    {
        typename T::is_sender;
    };

    struct connect_t {
        template <typename Sender, typename Rec>
        requires tag_invocable<connect_t, Sender, Rec>
        auto operator()(Sender &&sender, Rec &&rec) const
            noexcept(is_nothrow_tag_invocable_v<connect_t, Sender, Rec>)
                -> tag_invoke_result_t<connect_t, Sender, Rec>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<Rec>(rec));
        }
    };

    inline constexpr connect_t connect;

    template <typename Sender, typename Rec>
    using connect_result_t = tag_invoke_result_t<connect_t, Sender, Rec>;

    struct get_context_t {
        template <typename T>
        requires tag_invocable<get_context_t, T &>
        auto operator()(T &t) const
            noexcept(is_nothrow_tag_invocable_v<get_context_t, T &>)
                -> tag_invoke_result_t<get_context_t, T &>
        {
            return tag_invoke(*this, t);
        }
    };

    inline constexpr get_context_t get_context;

    struct CPU_context {
    };

    template <typename _Context = CPU_context>
    struct execution_receiver : _Context {
        using Context = _Context;

        friend Context &tag_invoke(Engine::Execution::get_context_t, execution_receiver &self)
        {
            return self;
        }
    };

    template <typename Rec>
    struct algorithm_receiver {
        using inner = Rec;

        template <typename... V>
        void set_value(V &&...value)
        {
            mRec.set_value(std::forward<V>(value)...);
        }

        void set_done()
        {
            mRec.set_done();
        }

        template <typename... R>
        void set_error(R &&...result)
        {
            mRec.set_error(std::forward<R>(result)...);
        }

        Rec mRec;

        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, algorithm_receiver &rec)
        {
            return f(rec.mRec);
        }
    };

    template <typename _Rec>
    struct base_state {
        using Rec = _Rec;

        base_state(Rec &&rec)
            : mRec(std::forward<Rec>(rec))
        {
        }

        template <typename... V>
        void set_value(V &&...value)
        {
            mRec.set_value(std::forward<V>(value)...);
        }
        void set_done()
        {
            mRec.set_done();
        }

        template <typename... R>
        void set_error(R &&...result)
        {
            mRec.set_error(std::forward<R>(result)...);
        }

        Rec mRec;

        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, base_state &state)
        {
            return f(state.mRec);
        }
    };

    template <typename State>
    struct algorithm_state_helper {

        using InnerRec = typename State::Rec;
        using Rec = typename InnerRec::inner;

        template <typename Sender, typename... Args>
        algorithm_state_helper(Sender &&sender, Rec &&rec, Args &&...args)
            : mState { connect(std::forward<Sender>(sender), InnerRec { std::forward<Rec>(rec), std::forward<Args>(args)... }) }
        {
        }

        ~algorithm_state_helper() { }

        void start()
        {
            mState.start();
        }

        template <typename... V>
        void set_value(Rec &rec, V &&...value)
        {
            rec.set_value(std::forward<V>(value)...);
        }

        void set_done(Rec &rec)
        {
            rec.set_done();
        }

        template <typename... R>
        void set_error(Rec &rec, R &&...result)
        {
            rec.set_error(std::forward<R>(result)...);
        }

        State mState;

        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, algorithm_state_helper &state)
        {
            return f(state.mState);
        }
    };

    template <typename Sender, typename InnerRec>
    using algorithm_state = algorithm_state_helper<connect_result_t<Sender, InnerRec>>;

    struct base_sender {
        using is_sender = void;
    };

    template <typename Sender>
    struct algorithm_sender : base_sender {
        using result_type = typename Sender::result_type;
        template <template <typename...> typename Tuple>
        using value_types = typename Sender::template value_types<Tuple>;
    };

}
}