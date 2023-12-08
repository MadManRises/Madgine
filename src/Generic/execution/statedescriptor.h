#pragma once

namespace Engine {
namespace Execution {

    namespace State {
        struct Text {
            std::string mText;
        };
    }

    using StateDescriptor = std::variant<State::Text>;

    struct visit_state_t {
        template <typename T, typename F>
        requires(!tag_invocable<visit_state_t, T &, F>) void operator()(T &, F &&f) const
        {
        }

        template <typename T, typename F>
        requires tag_invocable<visit_state_t, T &, F>
        auto operator()(T &t, F &&f) const
            noexcept(is_nothrow_tag_invocable_v<visit_state_t, T &, F>)
                -> tag_invoke_result_t<visit_state_t, T &, F>
        {
            return tag_invoke(*this, t, std::forward<F>(f));
        }
    };

    constexpr visit_state_t visit_state;

}
}