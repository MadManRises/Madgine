#include "python3lib.h"

#include "python3debugger.h"

#include "util/pymoduleptr.h"
#include "util/pyobjectutil.h"

#include "Meta/keyvalue/boundapifunction.h"
#include "Meta/keyvalue/functiontable.h"
#include "Meta/keyvalue/typedscopeptr.h"

#include "Meta/keyvalue/valuetype.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Generic/execution/algorithm.h"

#include "util/python3lock.h"

#include "Interfaces/filesystem/path.h"

#include <frameobject.h>

METATABLE_BEGIN(Engine::Scripting::Python3::Python3Debugger)
METATABLE_END(Engine::Scripting::Python3::Python3Debugger)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        extern int executionTrace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg);

        extern PyTypeObject PySuspendExceptionType;

        struct PyDebugLocationObject {
            PyObject_HEAD
                Python3DebugLocation mLocation;
            Debug::ParentLocation *mParent = nullptr;
            bool mResume = false;
            bool mSkipOnce = false;
        };

        PyTypeObject PyDebugLocationType = {
            .ob_base = PyObject_HEAD_INIT(NULL)
                           .tp_name
            = "PyDebugLocation",
            .tp_basicsize = sizeof(PyDebugLocationObject),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PyDebugLocationObject, &PyDebugLocationObject::mLocation>,
            .tp_doc = "Python3 wrapper for debug location",
            .tp_new = PyType_GenericNew,
        };

        std::string Python3DebugLocation::toString() const
        {
            return file().str() + "/" + module() + ":" + std::to_string(lineNr());
        }

        std::map<std::string_view, ValueType> Python3DebugLocation::localVariables() const
        {
            if (!mFrame->f_locals)
                return {};
            PyDictPtr locals = PyDictPtr::fromBorrowed(mFrame->f_locals);

            std::map<std::string_view, ValueType> results;

            std::ranges::transform(locals, std::inserter(results, results.end()), [](std::pair<PyObject *, PyObject *> p) {
                return std::make_pair(PyUnicode_AsUTF8(p.first), fromPyObject(p.second));
            });

            return results;
        }

        bool Python3DebugLocation::wantsPause(Debug::ContinuationType type) const
        {
            return true;
        }

        Filesystem::Path Python3DebugLocation::file() const
        {
            return PyUnicode_AsUTF8(mFrame->f_code->co_filename);
        }

        std::string Python3DebugLocation::module() const
        {
            return PyUnicode_AsUTF8(mFrame->f_code->co_name);
        }

        size_t Python3DebugLocation::lineNr() const
        {
            return PyCode_Addr2Line(mFrame->f_code, mFrame->f_lasti);
        }

        Python3Debugger::Guard::Guard(Debug::ParentLocation *parent)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyDebugLocationType, NULL);
            Python3DebugLocation *location = new (&reinterpret_cast<PyDebugLocationObject *>(obj)->mLocation) Python3DebugLocation;
            reinterpret_cast<PyDebugLocationObject *>(obj)->mParent = parent;
            PyEval_SetTrace(&Python3Debugger::trace, obj);
        }

        Python3Debugger::Guard::Guard(PyObjectPtr location)
        {
            PyEval_SetTrace(&Python3Debugger::trace, location);
        }

        Python3Debugger::Guard::~Guard()
        {
            PyEval_SetTrace(executionTrace, nullptr);
        }

        int Python3Debugger::trace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg)
        {
            assert(stackUnwindable());

            PyDebugLocationObject *location = reinterpret_cast<PyDebugLocationObject *>(obj);

            switch (event) {
            default:
                throw 0;
            case PyTrace_CALL:
                if (!location->mResume && !location->mSkipOnce) {
                    if (!location->mLocation.mFrame)
                        location->mLocation.stepInto(location->mParent);
                    location->mLocation.mFrame = frame;
                }
                break;

            case PyTrace_RETURN:
                if (arg) {
                    location->mLocation.mFrame = frame->f_back;
                    if (!location->mLocation.mFrame)
                        location->mLocation.stepOut(location->mParent);
                }
                location->mResume = frame->f_back;
                break;

            case PyTrace_EXCEPTION:
                if (PyTuple_GetItem(arg, 0) != (PyObject *)&PySuspendExceptionType) {
                    location->mLocation.mFrame = frame->f_back;
                    if (!location->mLocation.mFrame)
                        location->mLocation.stepOut(location->mParent);
                }
                location->mResume = frame->f_back;
                break;

            case PyTrace_LINE:
                if (location->mSkipOnce) {
                    location->mSkipOnce = false;
                    break;
                }
                if (!location->mLocation.pass(Debug::ContinuationType::Flow)) {
                    *frame->f_stacktop++ = suspend(
                        [frame, location](BehaviorReceiver &receiver, std::vector<PyFramePtr> frames, Closure<void(std::string_view)> out, std::stop_token st) {
                            PyFrameObject *frame = frames.front();
                            if (frame->f_lasti == -1)
                                frame->f_lasti = 0;
                            else
                                frame->f_lasti += sizeof(_Py_CODEUNIT);
                            location->mLocation.yield([location, out { std::move(out) }, &receiver, st](Debug::ContinuationMode mode, std::vector<PyFramePtr> frames) mutable {
                                Python3Lock lock { std::move(out), std::move(st) };
                                Guard guard { PyObjectPtr::fromBorrowed(reinterpret_cast<PyObject *>(location)) };
                                switch (mode) {
                                case Debug::ContinuationMode::Step:
                                case Debug::ContinuationMode::Resume:
                                    location->mSkipOnce = true;
                                    {
                                        PyFrameObject *frame = frames.front();
                                        if (frame->f_lasti == 0)
                                            frame->f_lasti = -1;
                                        else
                                            frame->f_lasti -= sizeof(_Py_CODEUNIT);
                                    }
                                    evalFrames(receiver, std::move(frames));
                                    break;
                                case Debug::ContinuationMode::Abort:
                                    frames.clear();
                                    receiver.set_done();
                                    break;
                                }
                            },
                                st, Debug::ContinuationType::Flow, std::move(frames));
                        });
                }

                break;

            case PyTrace_OPCODE:
                location->mResume = false;
                break;
            }

            return executionTrace(obj, frame, event, arg);
        }
    }
}
}
