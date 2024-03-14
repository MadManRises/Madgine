#pragma once

#include "algorithm.h"

namespace Engine {
namespace Execution {

    namespace State {
        struct Text;
        struct Progress;
        struct BeginBlock;
        struct EndBlock;
        struct PushDisabled;
        struct PopDisabled;
        struct SubLocation;
        struct Contextual;
    }

    using StateDescriptor = std::variant<State::Text, State::Progress, State::BeginBlock, State::EndBlock, State::PushDisabled, State::PopDisabled, State::SubLocation, State::Contextual>;

    namespace State {
        struct Text {
            std::string mText;
        };
        struct Progress {
            float mRatio;
        };
        struct BeginBlock {
            std::string mName;
        };
        struct EndBlock {
        };
        struct PushDisabled {
        };
        struct PopDisabled {
        };
        struct SubLocation {
        };
        struct Contextual {
            std::function<StateDescriptor(const void *)> mMapping;
        };
    }

    struct visit_state_t {
        template <typename T, typename F>
        requires(!tag_invocable<visit_state_t, T &, F>) void operator()(T &, F &&f) const
        {
            f(State::SubLocation {});
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

    template <typename F, typename... Sender>
    void tag_invoke(visit_state_t, sequence_t::sender<Sender...> &sender, F &&f)
    {
        TupleUnpacker::forEach(sender.mSenders, [&](auto &sender) {
            visit_state(sender, f);
        });
    }

}
}