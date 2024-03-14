#pragma once

#include "Generic/genericresult.h"

#include "Generic/execution/concepts.h"

#include "pyframeptr.h"

#include "Generic/closure.h"

#include "Madgine/behaviorreceiver.h"

#include "Madgine/state.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {
        void setupExecution();

        bool stackUnwindable();

        PyObject *evalFrame(PyFrameObject *frame, int throwExc);
        void evalFrame(BehaviorReceiver &receiver, PyFramePtr frame);
        void evalFrames(BehaviorReceiver &receiver, std::vector<PyFramePtr> frames);

        PyObject *suspend(Closure<void(BehaviorReceiver &, std::vector<PyFramePtr>, Closure<void(std::string_view)>, std::stop_token)> callback);

        BehaviorError fetchError();

        struct MADGINE_PYTHON3_EXPORT ExecutionState : BehaviorReceiver {
            ExecutionState(PyFramePtr frame, Closure<void(std::string_view)> out);
            ~ExecutionState();

            void start();

            PyFramePtr mFrame;
            Closure<void(std::string_view)> mOut;
        };

        struct ExecutionSender : Execution::base_sender {
            using result_type = BehaviorError;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<ArgumentList>;

            template <typename Rec>
            friend auto tag_invoke(Execution::connect_t, ExecutionSender &&sender, Rec &&rec)
            {
                return VirtualBehaviorState<Rec, ExecutionState> { std::forward<Rec>(rec), std::move(sender.mFrame), std::move(sender.mOut) };
            }

            MADGINE_PYTHON3_EXPORT friend void tag_invoke(Execution::visit_state_t, ExecutionSender &sender, CallableView<void(const Execution::StateDescriptor &)> visitor);

            PyFramePtr mFrame;
            Closure<void(std::string_view)> mOut;
        };

    }
}
}