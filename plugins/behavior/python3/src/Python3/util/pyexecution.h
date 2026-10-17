#pragma once

#include "Generic/execution/stoppable.h"

#include "Meta/reflect/result.h"

#include "Madgine/debug/debuggablesender.h"

#include "pyobjectptr.h"
#include "python3lock.h"

namespace Engine {
namespace Behavior {
    namespace Python3 {

        bool lock();
        void unlock();

        MADGINE_PYTHON3_EXPORT extern PyTypeObject PyDebugLineType;
        MADGINE_PYTHON3_EXPORT extern PyTypeObject PyStateType;

        MADGINE_PYTHON3_EXPORT extern PyObject *sPyContinuationContextVar;
        MADGINE_PYTHON3_EXPORT extern PyObject *sPyReceiverContextVar;

        struct DebugLine {

            size_t mLineNr;
        };

        struct PyDebugLine {
            PyObject_HEAD
                DebugLine mLine;
        };

        extern PyMethodDef PyStateMethods[];

        struct PyStateBase {

            ~PyStateBase();
            void resume();

            std::atomic_flag mFlag;
            Python3BehaviorState *mState = nullptr;            
            PyObjectPtr mResult;

            TypedPtr mChild;

            void (*mDestruct)(PyStateBase &) = nullptr;
        };

        struct PyStateHelper {
            PyObject_HEAD
                PyStateBase mState;
        };

        struct PyReceiver {

            void set_value(const Reflect::ArgumentList &values);
            void set_error(Reflect::Error error);
            void set_done();

            template <typename CPO, typename... Args>
                requires(is_tag_invocable_v<CPO, BehaviorReceiver &, Args...>)
            friend auto tag_invoke(CPO f, PyReceiver &rec, Args &&...args) noexcept(is_nothrow_tag_invocable_v<CPO, BehaviorReceiver &, Args...>)
                -> tag_invoke_result_t<CPO, BehaviorReceiver &, Args...>
            {
                return tag_invoke(f, rec.mReceiver, std::forward<Args>(args)...);
            }

            friend Debug::Continuation *tag_invoke(Debug::get_continuation_t, PyReceiver &rec)
            {
                return &rec.mContinuation;
            }

            friend TypedPtr &tag_invoke(Debug::get_debug_child_location_t, PyReceiver &rec)
            {
                return rec.mState.mChild;
            }

            PyStateBase &mState;
            BehaviorReceiver &mReceiver;
            Debug::Continuation &mContinuation;
        };

        template <typename Sender>
        struct PyState : PyStateBase {
            using Inner = Execution::connect_result_t<Execution::with_debug_location_t::sender<Execution::stoppable_t::sender<Sender>>, PyReceiver>;
            ManualLifetime<Inner> mInnerState;
        };

        template <typename Sender>
        struct PyStateWrapper {
            PyObject_HEAD
                PyState<Sender>
                    mState;
        };

        PyObject *PyState_Alloc(size_t size);

        template <typename Sender>
        PyObject *PyAwait(Sender &&sender)
        {
            PyObject *obj = PyState_Alloc(sizeof(PyStateWrapper<Sender>));
            PyObjectPtr pyContinuation;
            if (PyContextVar_Get(sPyContinuationContextVar, NULL, &pyContinuation) < 0)
                return nullptr;
            Debug::Continuation *continuation = static_cast<Debug::Continuation *>(PyCapsule_GetPointer(pyContinuation, "Continuation"));
            if (!continuation)
                return nullptr;
            PyObjectPtr pyReceiver;
            if (PyContextVar_Get(sPyReceiverContextVar, NULL, &pyReceiver) < 0)
                return nullptr;
            BehaviorReceiver *receiver = static_cast<BehaviorReceiver *>(PyCapsule_GetPointer(pyReceiver, "Receiver"));
            if (!receiver)
                return nullptr;
            Python3Suspend suspend;
            PyState<Sender> *state = &reinterpret_cast<PyStateWrapper<Sender> *>(obj)->mState;
            new (state) PyState<Sender>;

            construct(state->mInnerState, DelayedConstruct { [&]() { return Execution::connect(std::forward<Sender>(sender) | Execution::stoppable | Execution::with_debug_location(), PyReceiver { *state, *receiver, *continuation }); } });
            state->mDestruct = [](PyStateBase &state) { destruct(static_cast<PyState<Sender> &>(state).mInnerState); };
            state->mInnerState->start();

            return obj;
        }

    }
}
}