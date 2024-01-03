#include "../python3lib.h"

#include "pyexecution.h"

#include "python3lock.h"

#include "../python3debugger.h"

#include <frameobject.h>

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
            Lambda<void(KeyValueReceiver &, std::vector<PyFramePtr>, Lambda<void(std::string_view)>)> mCallback;
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

        void evalFrame(KeyValueReceiver &receiver, PyFramePtr frame)
        {
            std::vector<PyFramePtr> frames;
            frames.push_back(std::move(frame));
            evalFrames(receiver, std::move(frames));
        }

        void evalFrames(KeyValueReceiver &receiver, std::vector<PyFramePtr> frames)
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

                    Lambda<void(KeyValueReceiver &, std::vector<PyFramePtr>, Lambda<void(std::string_view)>)> callback = std::move(suspend.mCallback);
                    std::vector<PyFramePtr> suspendedFrames = std::move(suspend.mFrames);

                    Py_DECREF(result);

                    Python3Unlock unlock;      

                    std::ranges::move(frames, std::back_inserter(suspendedFrames));
                    frames.clear();

                    callback(receiver, std::move(suspendedFrames), unlock.out());

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

        PyObject *suspend(Lambda<void(KeyValueReceiver &, std::vector<PyFramePtr>, Lambda<void(std::string_view)>)> callback)
        {
            assert(stackUnwindable());

            PyObject *suspendEx = PyObject_CallObject((PyObject *)&PySuspendExceptionType, nullptr);
            PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(static_cast<PyObject *>(suspendEx))->mException;
            new (&suspend) PySuspendException;

            suspend.mCallback = std::move(callback);

            return suspendEx;
        }

        void setupExecution()
        {
            PyGILState_GetThisThreadState()->interp->eval_frame = &evalFrame;

            PyEval_SetTrace(&executionTrace, nullptr);
        }

    }
}
}