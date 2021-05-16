#include "../python3lib.h"

#include "pyobjectptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        PyObjectPtr::PyObjectPtr(PyObject *object)
            : mObject(object)
        {
        }

        PyObjectPtr::PyObjectPtr(PyObjectPtr &&other)
            : mObject(std::exchange(other.mObject, nullptr))
        {
        }

        PyObjectPtr::~PyObjectPtr()
        {
            reset();
        }

        PyObjectPtr PyObjectPtr::get(const std::string_view &name) const
        {
            return PyObject_GetAttrString(mObject, name.data());
        }

        PyObjectPtr PyObjectPtr::call(const std::string_view &name, const char *format, ...) const
        {
            va_list vl;
            va_start(vl, format);
            PyObjectPtr result = call(name, Py_VaBuildValue(format, vl));
            va_end(vl);
            return result;
        }

        PyObjectPtr PyObjectPtr::call(const std::string_view &name, const PyObjectPtr &args) const
        {
            return PyObject_CallObject(get(name), args);
        }

        void PyObjectPtr::reset()
        {
            Py_XDECREF(mObject);
            mObject = nullptr;
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

    }
}
}
