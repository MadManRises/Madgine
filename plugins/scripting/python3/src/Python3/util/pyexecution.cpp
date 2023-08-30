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
            PyObjectPtr mSender;
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

        void PyDeallocSender(PyObject *self)
        // destruct the object
        {
            fromPySender(self).~PySenderBase();
            Py_TYPE(self)->tp_free(self);
        }

        PyTypeObject PySenderType = {
            .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                           .tp_name
            = "PySender",
            .tp_basicsize = sizeof(PyVarObject),
            .tp_itemsize = 1,
            .tp_dealloc = &PyDeallocSender,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Allows storing any sender within a Python object",
            .tp_new = PyType_GenericNew,
        };

        void PySenderReceiver::set_value()
        {
            Python3Lock lock { mSender.mOut };
            evalFrames(*mSender.mReceiver, std::move(mSender.mFrames));

            Py_DECREF(mSender.getSelf());
        }

        void PySenderReceiver::set_value(const ArgumentList &args)
        {
            *static_cast<PyFrameObject *>(mSender.mFrames.front())->f_stacktop++ = toPyObject(args[0]);
            set_value();
        }

        void PySenderReceiver::set_error(GenericResult)
        {
            throw 0;

            Py_DECREF(mSender.getSelf());
        }

        void PySenderReceiver::set_done()
        {
            throw 0;

            Py_DECREF(mSender.getSelf());
        }

        void PySenderBase::connectAndStart(KeyValueReceiver &receiver, std::streambuf *out, std::vector<PyFramePtr> outerFrames)
        {
            std::ranges::move(outerFrames, std::back_inserter(mFrames));
            mReceiver = &receiver;
            mOut = out;

            Py_INCREF(getSelf());
            start();
        }

        PyObject *PySenderBase::getSelf()
        {
            return reinterpret_cast<PyObject *>(reinterpret_cast<PyVarObject *>(this) - 1);
        }

        void PySenderBase::addFrame(PyFramePtr frame)
        {
            mFrames.push_back(std::move(frame));
        }

        PySenderBase &fromPySender(PyObject *sender)
        {
            assert(sender->ob_type == &PySenderType);
            return reinterpret_cast<PySenderBase &>(reinterpret_cast<PySenderObject *>(sender)->mSender);
        }

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

                fromPySender(suspend.mSender).addFrame(PyFramePtr::fromBorrowed(frame));

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

                    PyObjectPtr senderObj = std::move(suspend.mSender);

                    Py_DECREF(result);

                    Python3Unlock unlock;                    

                    PySenderBase &sender = fromPySender(senderObj);
                    sender.connectAndStart(receiver, unlock.out(), std::move(frames));                    

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

        PyObject *suspend(PyObject *sender)
        {
            assert(stackUnwindable());

            PyObject *suspendEx = PyObject_CallObject((PyObject *)&PySuspendExceptionType, nullptr);
            PySuspendException &suspend = reinterpret_cast<PySuspendExceptionObject *>(static_cast<PyObject *>(suspendEx))->mException;
            new (&suspend) PySuspendException;

            suspend.mSender = sender;

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