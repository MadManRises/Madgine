#include "../python3lib.h"

#include "pyobjectptr.h"

#include "pydictptr.h"

#include "pyobjectutil.h"

#include "Meta/keyvalue/valuetype.h"

#include <traceback.h>
#include <frameobject.h>

namespace Engine {
namespace Scripting {
    namespace Python3 {

        PyObjectPtr::PyObjectPtr(PyObject *object)
            : mObject(object)
        {
        }

        PyObjectPtr::PyObjectPtr(const PyObjectPtr &other)
            : mObject(other.mObject)
        {
            Py_INCREF(mObject);
        }

        PyObjectPtr::PyObjectPtr(PyObjectPtr &&other)
            : mObject(std::exchange(other.mObject, nullptr))
        {
        }

        PyObjectPtr::~PyObjectPtr()
        {
            reset();
        }

        PyObjectPtr PyObjectPtr::get(std::string_view name) const
        {
            return PyObject_GetAttrString(mObject, name.data());
        }

        PyObjectPtr PyObjectPtr::call(const ArgumentList &args) const
        {
            PyObjectPtr tuple = PyTuple_New(args.size());
            for (size_t i = 0; i < args.size(); ++i) {
                PyTuple_SetItem(tuple, i, toPyObject(args[i]));
            }

            return call(tuple, PyObjectPtr {});
        }

        PyObjectPtr PyObjectPtr::call(const char *format, ...) const
        {
            va_list vl;
            va_start(vl, format);
            PyObjectPtr result = call(Py_VaBuildValue(format, vl), PyObjectPtr {});
            va_end(vl);
            return result;
        }

        PyObjectPtr PyObjectPtr::call(const PyDictPtr &kwargs, const char *format, ...) const
        {
            va_list vl;
            va_start(vl, format);
            PyObjectPtr result = call(Py_VaBuildValue(format, vl), kwargs);
            va_end(vl);
            return result;
        }

        PyObjectPtr PyObjectPtr::call(const PyObjectPtr &args, const PyObjectPtr &kwargs) const
        {
            return PyObject_Call(mObject, args, kwargs);
        }

        PyObjectFieldAccessor PyObjectPtr::operator[](const PyObjectPtr &name) const
        {
            return { *this, name };
        }

        void PyObjectPtr::reset()
        {
            Py_XDECREF(mObject);
            mObject = nullptr;
        }

        void PyObjectPtr::handleError()
        {
            if (!mObject) {
                PyObjectPtr type, value, traceback;
                PyErr_Fetch(&type, &value, &traceback);

                PyTracebackObject *tb = reinterpret_cast<PyTracebackObject*>(static_cast<PyObject*>(traceback));
                while (tb->tb_next)
                    tb = tb->tb_next;
                
                const char *filename = PyUnicode_AsUTF8(tb->tb_frame->f_code->co_filename);
                size_t line = tb->tb_frame->f_code->co_firstlineno;

                const char *errorMessage = PyUnicode_AsUTF8(value);
                Engine::Util::LogDummy { Engine::Util::MessageType::ERROR_TYPE, filename, line } << "Unhandled Python Exception:\n"
                                                                                           << errorMessage;
            }
        }

        PyObjectPtr &PyObjectPtr::operator=(PyObjectPtr &&other)
        {
            std::swap(mObject, other.mObject);
            return *this;
        }

        PyObjectPtr::operator bool() const
        {
            return mObject;
        }

        PyObjectPtr::operator PyObject *() const
        {
            return mObject;
        }

        PyObject **PyObjectPtr::operator&()
        {
            assert(!mObject);
            return &mObject;
        }

        PyObjectPtr PyObjectPtr::None()
        {
            Py_RETURN_NONE;
        }

        PyObjectFieldAccessor &PyObjectFieldAccessor::operator=(const PyObjectPtr &value)
        {
            auto result = PyObject_SetItem(mObject, mKey, value);
            assert(result == 0);
            return *this;
        }

    }
}
}
