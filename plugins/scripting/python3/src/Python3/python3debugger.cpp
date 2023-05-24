#include "python3lib.h"

#include "python3debugger.h"

#include "util/pymoduleptr.h"
#include "util/pyobjectutil.h"

#include "Meta/keyvalue/boundapifunction.h"
#include "Meta/keyvalue/functiontable.h"
#include "Meta/keyvalue/typedscopeptr.h"

#include "Meta/keyvalue/valuetype.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/debug/debugger.h"

#include "Generic/execution/algorithm.h"

#include "util/python3lock.h"

#include <frameobject.h>

METATABLE_BEGIN(Engine::Scripting::Python3::Python3Debugger)
METATABLE_END(Engine::Scripting::Python3::Python3Debugger)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        extern PyTypeObject PySuspendExceptionType;

        static bool sResume = false;
        static bool sSkipOnce = false;

        struct Python3DebugLocation : Debug::DebugLocation {

            Python3DebugLocation(PyFrameObject *frame)
                : mFrame(frame)
            {
            }

            virtual std::string toString() const override
            {
                int line = PyCode_Addr2Line(mFrame->f_code, mFrame->f_lasti) + 1;
                return std::string { PyUnicode_AsUTF8(mFrame->f_code->co_filename) } + "/" + std::string { PyUnicode_AsUTF8(mFrame->f_code->co_name) } + ":" + std::to_string(line);
            }

            virtual std::map<std::string_view, ValueType> localVariables() const override
            {
                PyDictPtr locals = PyDictPtr::fromBorrowed(mFrame->f_locals);

                std::map<std::string_view, ValueType> results;

                std::ranges::transform(locals, std::inserter(results, results.end()), [](std::pair<PyObject *, PyObject *> p) {
                    return std::make_pair(PyUnicode_AsUTF8(p.first), fromPyObject(p.second));
                });

                return results;
            }

            PyFrameObject *mFrame;
        };

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
                    //if (PyTuple_GetItem(arg, 0) != (PyObject *)&PySuspendExceptionType)
                    //    Debug::Debugger::getSingleton().stepOut(frame);
                    break;

                case PyTrace_LINE:
                    if (sSkipOnce) {
                        sSkipOnce = false;
                        break;
                    }

                    *frame->f_stacktop++ = suspend(Debug::Debugger::getSingleton().yield(frame) | Execution::then([]() { sSkipOnce = true; }));
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
