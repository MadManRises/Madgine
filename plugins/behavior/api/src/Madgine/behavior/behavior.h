#pragma once

#include "Generic/delayedconstruct.h"
#include "Generic/functor.h"

#include "behaviorcoroutine.h"
#include "behaviorreceiver.h"

namespace Engine {
namespace Behavior {

    struct MADGINE_BEHAVIOR_EXPORT Behavior {

        static void destroyState(BehaviorStateBase *state);
        using StatePtr = std::unique_ptr<BehaviorStateBase, Functor<&destroyState>>;

        Behavior() = default;
        Behavior(StatePtr state);

        template <Execution::AnySender Sender>
        Behavior(Sender &&sender)
            : mState(new SenderBehaviorState<Sender>(std::forward<Sender>(sender)))
        {
        }

        Behavior &operator=(StatePtr state);

        StatePtr release()
        {
            return std::move(mState);
        }

        StatePtr connect(BehaviorReceiver &receiver);

        struct MADGINE_BEHAVIOR_EXPORT state : BehaviorReceiver {

            state(StatePtr state);

            void start();
            void stop();

            friend void tag_invoke(Execution::visit_state_t, state *state, auto &&visitor)
            {
                if (state)
                    visitor(Execution::State::DebugLocation { state->mChild });
                else
                    visitor(Execution::State::Text { "Behavior" });
            }

            TypedPtr mChild;

        protected:           
            StatePtr mState;
        };

        template <typename Rec>
        struct rec_helper : Execution::algorithm_receiver<Rec> {

            friend TypedPtr &tag_invoke(Debug::get_debug_child_location_t, rec_helper &rec)
            {
                return rec.mState.mChild;
            }

            state &mState;
        };

        template <typename Rec>
        struct state_helper : VirtualBehaviorState<rec_helper<Rec>, state> {
            state_helper(Rec &&rec, StatePtr statePtr)
                : VirtualBehaviorState<rec_helper<Rec>, state> { { std::forward<Rec>(rec), *this }, std::move(statePtr) }
            {
            }
        };

        using is_sender = void;

        using result_type = Reflect::Error;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<Reflect::ArgumentList>;

        template <typename Rec, std::same_as<Behavior> T> // Necessary to prevent implicit conversion
        friend auto tag_invoke(Execution::connect_t, T &&behavior, Rec &&rec)
        {
            assert(behavior.mState);
            return state_helper<Rec> { std::forward<Rec>(rec), std::move(behavior.mState) };
        }

        using promise_type = CoroutineBehaviorState;

        StatePtr mState;
    };

    template <Execution::AnySender Sender>
    struct SenderBehaviorState : BehaviorStateBase {

        using State = Execution::connect_result_t<Execution::with_debug_location_t::sender<Sender>, Execution::deferred_receiver<BehaviorReceiver>&>;

        SenderBehaviorState(Sender &&sender)
            : mState(Execution::connect(std::forward<Sender>(sender) | Execution::with_debug_location(), mReceiver))
        {
        }

        void start() override
        {

            mState.start();
        }

        void stop() override
        {
            mState.stop();
        }

        State mState;        
    };

}
}