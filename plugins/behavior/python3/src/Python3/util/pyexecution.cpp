#include "../python3lib.h"

#include "pyexecution.h"

#include "Meta/reflect/objectptr.h"

#include "../python3behaviors.h"
#include "pydictptr.h"
#include "pylistptr.h"
#include "pyobjectinstance.h"
#include "pyobjectutil.h"
#include "pyscopeptr.h"
#include "pysender.h"
#include "python3lock.h"

///  @cond

struct _frame { }; // HACK for TypedPtr

///  @endcond

namespace Engine {
namespace Behavior {
    namespace Python3 {

        PyObject *sPyContinuationContextVar = nullptr;
        PyObject *sPyReceiverContextVar = nullptr;

        bool lock()
        {
            // assert(PyGILState_Check() == 0);
            [[maybe_unused]] PyGILState_STATE handle = PyGILState_Ensure();
            assert(PyGILState_Check() == 1);
            return handle == PyGILState_UNLOCKED;
        }

        void unlock()
        {
            assert(PyGILState_Check() == 1);
            PyGILState_Release(PyGILState_UNLOCKED);
        }

        int PyDebugLine_init(PyDebugLine *self, PyObject *args, PyObject *kwds)
        {
            new (&self->mLine) DebugLine;
            return PyArg_ParseTuple(args, "n", &self->mLine.mLineNr);
        }

        PyObject *PyDebugLine_await(PyObject *self)
        {
            Py_IncRef(self);
            return self;
        }

        PyObject *PyDebugLine_next(PyDebugLine *self)
        {
            PyObjectPtr pyReceiver;
            if (PyContextVar_Get(sPyReceiverContextVar, NULL, &pyReceiver) < 0)
                return nullptr;
            BehaviorReceiver *rec = static_cast<BehaviorReceiver *>(PyCapsule_GetPointer(pyReceiver, "Receiver"));
            if (!rec)
                return nullptr;
            Debug::ContextInfo &context = Debug::get_debug_context(*rec);
            DebugLine &line = self->mLine;

            PyObjectPtr continuation;
            if (PyContextVar_Get(sPyContinuationContextVar, NULL, &continuation) < 0)
                return nullptr;
            Debug::Continuation *cont = static_cast<Debug::Continuation *>(PyCapsule_GetPointer(continuation, "Continuation"));
            if (!cont)
                return nullptr;

            if (line.mLineNr > 0 && (context.wantsPause(PyEval_GetFrame(), Debug::ContinuationType::Flow, line.mLineNr) || cont->mode() != Debug::ContinuationMode::Continue)) {
                line.mLineNr = 0;
                PyObject *selfObj = reinterpret_cast<PyObject *>(self);
                Py_IncRef(selfObj);
                return selfObj;
            } else {
                PyErr_SetNone(PyExc_StopIteration);
                return nullptr;
            }
        }

        PyAsyncMethods PyDebugLineAsyncMethods = {
            .am_await = PyDebugLine_await
        };

        PyTypeObject PyDebugLineType = {
            .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Engine.DebugLine",
            .tp_basicsize = sizeof(PyDebugLine),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PyDebugLine, &PyDebugLine::mLine>,
            .tp_as_async = &PyDebugLineAsyncMethods,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Python helper for debugging",
            .tp_iternext = (iternextfunc)PyDebugLine_next,
            .tp_init = (initproc)PyDebugLine_init,
            .tp_new = PyType_GenericNew,
        };

        PyStateBase::~PyStateBase()
        {
            assert(!mDestruct);
            if (mDestruct)
                mDestruct(*this);
        }

        void PyStateBase::resume()
        {
            if (mFlag.test_and_set()) {
                mState->resume(std::move(mResult));
            }
        }

        void PyReceiver::set_value(const Reflect::ArgumentList &values)
        {
            assert(mState.mDestruct);
            mState.mDestruct(mState);
            mState.mDestruct = nullptr;

            Python3Lock lock;
            mState.mResult = toPyTuple(values);
            if (!mState.mResult)
                mState.mResult = PyErr_GetRaisedException();

            mState.resume();
        }

        void PyReceiver::set_error(Reflect::Error error)
        {
            assert(mState.mDestruct);
            mState.mDestruct(mState);
            mState.mDestruct = nullptr;

            Python3Lock lock;
            mState.mResult = toPyException(std::move(error));

            mState.resume();
        }

        void PyReceiver::set_done()
        {
            assert(mState.mDestruct);
            mState.mDestruct(mState);
            mState.mDestruct = nullptr;

            Python3Lock lock;
            mState.mResult.reset();

            mState.resume();
        }

        PyObject *PyState_send(PyStateHelper *self,
            PyObject *const *args,
            Py_ssize_t nargs)
        {
            [[maybe_unused]] PyStateBase &state = self->mState;
            assert(state.mFlag.test());

            if (PyExceptionInstance_Check(args[0])) {
                Py_IncRef(args[0]);
                PyErr_SetRaisedException(args[0]);
            } else {
                PyObjectPtr emptyTuple = PyTuple_New(0);
                PyObject *exc = PyObject_Call(PyExc_StopIteration, emptyTuple, NULL);
                PyObject_SetAttrString(exc, "value", args[0]);

                PyErr_SetObject(PyExc_StopIteration, exc);
            }
            return nullptr;
        }

        PyObject *PyState_next(PyStateHelper *self)
        {
            PyStateBase &state = self->mState;
            if (state.mFlag.test()) {
                PyObject *args = state.mResult;
                return PyState_send(self, &args, 1);
            } else {
                Py_INCREF(self);
                return (PyObject *)self;
            }
        }

        PyMethodDef PyStateMethods[] = {
            { "send", (PyCFunction)PyState_send, METH_FASTCALL, "" },
            { NULL, NULL, 0, NULL } /* Sentinel */
        };

        PyObject *PyState_Alloc(size_t size)
        {
            void *mem = PyObject_Malloc(size);

            return PyObject_Init(reinterpret_cast<PyObject *>(mem), &PyStateType);
        }

        void PyState_Dealloc(PyStateHelper *state)
        {
            state->mState.~PyStateBase();
            PyObject_Free(state);
        }

        PyTypeObject PyStateType = {
            .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Engine.ExecutionState",
            .tp_basicsize = sizeof(PyStateBase),
            .tp_itemsize = 0,
            .tp_dealloc = (destructor)PyState_Dealloc,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "helper for Execution states",
            .tp_iternext = (iternextfunc)PyState_next,
            .tp_methods = PyStateMethods,
            .tp_new = PyType_GenericNew,
        };

    }
}
}