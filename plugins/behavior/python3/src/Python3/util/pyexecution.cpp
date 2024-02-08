#include "../python3lib.h"

#include "pyexecution.h"

#include "python3lock.h"

#include "../python3debugger.h"

#include <frameobject.h>

#include "pydictptr.h"
#include "pyobjectutil.h"

#include "Meta/keyvalue/argumentlist.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static bool sUnwindable = false;

        struct PySuspendException {
            PyObject **mStacktop;
            int mBlock;
            Closure<void(BehaviorReceiver &, std::vector<PyFramePtr>, Closure<void(std::string_view)>, std::stop_token)> mCallback;
            std::vector<PyFramePtr> mFrames;
        };

        struct PySuspendExceptionObject {
            PyException_HEAD
                PySuspendException mException;
        };

        PyTypeObject PySuspendExceptionType = {
            .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                           .tp_name
            = "PySuspendException",
            .tp_basicsize = sizeof(PySuspendExceptionObject),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PySuspendExceptionObject, &PySuspendExceptionObject::mException>,
            .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASE_EXC_SUBCLASS,
            .tp_doc = "Utility exception for Python suspension",
            .tp_base = (PyTypeObject *)PyExc_Exception,
            .tp_new = PyType_GenericNew,
        };

        struct PyBehaviorScope {
            BehaviorReceiver *mReceiver;
            PyDictPtr mInner;
        };

        struct PyBehaviorScopeObject {
            PyObject_HEAD
                PyBehaviorScope mScope;
        };

        static PyObject *
        PyBehaviorScope_subscript(PyBehaviorScopeObject *self, PyObject *key)
        {
            const char *name;

            if (!PyArg_Parse(key, "s", &name))
                return NULL;

            ValueType v;
            bool found = self->mScope.mReceiver->resolveVar(name, v);
            if (found) {
                return toPyObject(v);
            } else {
                return PyObject_GetItem(self->mScope.mInner, key);
            }
        }

        /* static PyObject *OwnedScopePtr_iter(const ScopePtr &p)
        {
            if (!p) {
                PyErr_SetString(PyExc_TypeError, "Nullptr is not iterable!");
                return NULL;
            }
            ScopeIterator proxyIt = p.find("__proxy");
            if (proxyIt != p.end()) {
                ValueType proxy;
                proxyIt->value(proxy);
                if (proxy.is<ScopePtr>()) {
                    return OwnedScopePtr_iter(proxy.as<ScopePtr>());
                }
            }
            return toPyObject(p.begin());
        }

        static PyObject *
        PyOwnedScopePtr_iter(PyBehaviorScopeObject *self)
        {
            return OwnedScopePtr_iter(self->mPtr.get());
        }*/

        static PyMappingMethods PyBehaviorScopeMethods = {
            .mp_subscript = (binaryfunc)&PyBehaviorScope_subscript,
        };

        PyTypeObject PyBehaviorScopeType = {
            .ob_base = PyObject_HEAD_INIT(NULL).tp_name = "Engine.BehaviorScope",
            .tp_basicsize = sizeof(PyBehaviorScopeObject),
            .tp_dealloc = &PyDealloc<PyBehaviorScopeObject, &PyBehaviorScopeObject::mScope>,
            //.tp_getattro = (getattrofunc)PyBehaviorScope_get,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Scope wrapper to access Behavior scope variables",
            //.tp_iter = (getiterfunc)PyOwnedScopePtr_iter,
            .tp_new = PyType_GenericNew,
            .tp_as_mapping = &PyBehaviorScopeMethods,
        };

        PyObject *evalFrame(PyFrameObject *frame, int throwExc)
        {
            frame->f_trace_opcodes = true;
            PyObject *result = _PyEval_EvalFrameDefault(frame, throwExc);
            if (!result && PyErr_ExceptionMatches((PyObject *)&PySuspendExceptionType)) {
                PyObjectPtr type, value, traceback;
                PyErr_Fetch(&type, &value, &traceback);

                PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(static_cast<PyObject *>(value))->mException;
                frame->f_stacktop = suspend.mStacktop;
                frame->f_iblock = suspend.mBlock;
                suspend.mStacktop = nullptr;
                suspend.mBlock = 0;

                suspend.mFrames.push_back(PyFramePtr::fromBorrowed(frame));

                return value.release();
            }
            return result;
        }

        void evalFrame(BehaviorReceiver &receiver, PyFramePtr frame)
        {
            std::vector<PyFramePtr> frames;
            frames.push_back(std::move(frame));
            evalFrames(receiver, std::move(frames));
        }

        void evalFrames(BehaviorReceiver &receiver, std::vector<PyFramePtr> frames)
        {
            assert(sUnwindable == false);
            sUnwindable = true;
            while (!frames.empty()) {
                auto it = frames.begin();
                PyFramePtr frame = std::move(*it);
                frames.erase(it);
                PyObject *result = PyEval_EvalFrame(frame);
                if (result && result->ob_type == &PySuspendExceptionType) {
                    PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(result)->mException;

                    assert(suspend.mStacktop == nullptr);
                    assert(suspend.mBlock == 0);

                    Closure<void(BehaviorReceiver &, std::vector<PyFramePtr>, Closure<void(std::string_view)>, std::stop_token)> callback = std::move(suspend.mCallback);
                    std::vector<PyFramePtr> suspendedFrames = std::move(suspend.mFrames);

                    Py_DECREF(result);

                    Python3Unlock unlock;

                    std::ranges::move(frames, std::back_inserter(suspendedFrames));
                    frames.clear();

                    callback(receiver, std::move(suspendedFrames), unlock.out(), unlock.st());

                } else {
                    if (frames.empty()) {
                        fromPyObject(receiver, result);
                    } else {
                        *static_cast<PyFrameObject *>(frames.front())->f_stacktop++ = result;
                    }
                }
            }
            sUnwindable = false;
        }

        int executionTrace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg)
        {

            if (event == PyTrace_OPCODE) {

                if (frame->f_stacktop > frame->f_valuestack && frame->f_stacktop[-1]->ob_type == &PySuspendExceptionType) {
                    PyObject *suspendEx = frame->f_stacktop[-1];
                    --frame->f_stacktop;

                    PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(suspendEx)->mException;

                    assert(suspend.mStacktop == nullptr);
                    assert(suspend.mBlock == 0);

                    if (frame->f_lasti == 0)
                        frame->f_lasti = -1;
                    else
                        frame->f_lasti -= sizeof(_Py_CODEUNIT);
                    suspend.mStacktop = frame->f_stacktop;
                    frame->f_stacktop = frame->f_valuestack;
                    suspend.mBlock = frame->f_iblock;
                    frame->f_iblock = 0;

                    PyErr_SetObject((PyObject *)&PySuspendExceptionType, suspendEx);
                    Py_DECREF(suspendEx);
                    return -1;
                }
            }
            return 0;
        }

        bool stackUnwindable()
        {
            return sUnwindable;
        }

        PyObject *suspend(Closure<void(BehaviorReceiver &, std::vector<PyFramePtr>, Closure<void(std::string_view)>, std::stop_token)> callback)
        {
            assert(stackUnwindable());

            PyObject *suspendEx = PyObject_CallObject((PyObject *)&PySuspendExceptionType, nullptr);
            PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(static_cast<PyObject *>(suspendEx))->mException;
            new (&suspend) PySuspendException;

            suspend.mCallback = std::move(callback);

            return suspendEx;
        }

        BehaviorError fetchError()
        {
            PyObjectPtr type, value, traceback;
            PyErr_Fetch(&type, &value, &traceback);

            const char *filename = "";
            size_t line = 0;

            if (traceback) {
                PyTracebackObject *tb = reinterpret_cast<PyTracebackObject *>(static_cast<PyObject *>(traceback));
                while (tb->tb_next)
                    tb = tb->tb_next;

                filename = PyUnicode_AsUTF8(tb->tb_frame->f_code->co_filename);
                line = tb->tb_frame->f_code->co_firstlineno;
            }

            PyObjectPtr str = PyObject_Str(value);
            const char *errorMessage = PyUnicode_AsUTF8(str);

            std::string msg;
            if (errorMessage)
                msg = errorMessage;

            return { BehaviorResult::UNKNOWN_ERROR, msg, filename, line };
        }

        void setupExecution()
        {
            PyGILState_GetThisThreadState()->interp->eval_frame = &evalFrame;

            PyEval_SetTrace(&executionTrace, nullptr);
        }

        ExecutionState::ExecutionState(PyFramePtr frame, Closure<void(std::string_view)> out)
            : mFrame(std::move(frame))
            , mOut(std::move(out))
        {
        }

        ExecutionState::~ExecutionState()
        {
            if (mFrame) {
                Python3Lock lock;
                mFrame.reset();
            }
        }

        void ExecutionState::start()
        {
            Python3Lock lock { std::move(mOut), stopToken() };

            Python3Debugger::Guard guard { debugLocation() };

            PyObject *wrapped = PyObject_CallObject((PyObject *)&PyBehaviorScopeType, NULL);
            new (&reinterpret_cast<PyBehaviorScopeObject *>(wrapped)->mScope) PyBehaviorScope { this, static_cast<PyFrameObject *>(mFrame)->f_globals };
            static_cast<PyFrameObject *>(mFrame)->f_globals = wrapped;

            evalFrame(*this, std::move(mFrame));
        }

        void tag_invoke(Execution::visit_state_t, ExecutionSender &sender, CallableView<void(const Execution::StateDescriptor &)> visitor)
        {
            visitor(Execution::State::SubLocation {});
        }

    }
}
}