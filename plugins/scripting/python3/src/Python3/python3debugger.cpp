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

        extern PyTypeObject PySuspendExceptionType;

        static bool sResume = false;
        static bool sSkipOnce = false;

        Python3DebugLocation::Python3DebugLocation(PyFrameObject *frame)
            : mFrame(frame)
        {
        }

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

        void Python3Debugger::setup()
        {
            PyEval_SetTrace(&Python3Debugger::trace, nullptr);
        }

        extern int executionTrace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg);
        int Python3Debugger::trace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg)
        {
            if (stackUnwindable()) {

                switch (event) {
                default:
                    throw 0;
                case PyTrace_CALL:
                    if (!sResume && !sSkipOnce)
                        Debug::Debugger::getSingleton().stepInto(frame, std::make_unique<Python3DebugLocation>(frame), frame->f_back);
                    break;

                case PyTrace_RETURN:
                    if (arg)
                        Debug::Debugger::getSingleton().stepOut(frame);
                    sResume = frame->f_back;
                    break;

                case PyTrace_EXCEPTION:
                    if (PyTuple_GetItem(arg, 0) != (PyObject *)&PySuspendExceptionType)
                        Debug::Debugger::getSingleton().stepOut(frame);
                    sResume = frame->f_back;
                    break;

                case PyTrace_LINE:
                    if (sSkipOnce) {
                        sSkipOnce = false;
                        break;
                    }

                    if (!Debug::Debugger::getSingleton().pass(frame)) {
                        *frame->f_stacktop++ = suspend(
                            [frame](KeyValueReceiver &receiver, std::vector<PyFramePtr> frames, Lambda<void(std::string_view)> out) {
                                PyFrameObject *frame = frames.front();
                                if (frame->f_lasti == -1)
                                    frame->f_lasti = 0;
                                else
                                    frame->f_lasti += sizeof(_Py_CODEUNIT);
                                Debug::Debugger::getSingleton().yield(frame, [out { std::move(out) }, &receiver, frames { std::move(frames) }]() mutable {
                                    Python3Lock lock { std::move(out) };
                                    sSkipOnce = true;
                                    PyFrameObject *frame = frames.front();
                                    if (frame->f_lasti == 0)
                                        frame->f_lasti = -1;
                                    else
                                        frame->f_lasti -= sizeof(_Py_CODEUNIT);
                                    evalFrames(receiver, std::move(frames));
                                });
                            });
                    }

                    break;

                case PyTrace_OPCODE:
                    sResume = false;
                    break;
                }
            }

            return executionTrace(obj, frame, event, arg);
        }

    }
}
}
