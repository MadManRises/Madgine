#pragma once

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct PyObjectFieldAccessor;
        struct PyDictPtr;

        struct MADGINE_PYTHON3_EXPORT PyObjectPtr {

            PyObjectPtr() = default;
            PyObjectPtr(PyObject *object);
            PyObjectPtr(const PyObjectPtr &other);
            PyObjectPtr(PyObjectPtr &&other);
            ~PyObjectPtr();

            PyObjectPtr get(std::string_view name) const;
            PyObjectPtr call(const ArgumentList &args) const;
            PyObjectPtr call(const char *format, ...) const;
            PyObjectPtr call(const PyDictPtr &kwargs, const char *format, ...) const;
            PyObjectPtr call(const PyObjectPtr &args, const PyObjectPtr &kwargs) const;

            PyObjectFieldAccessor operator[](const PyObjectPtr &name) const;

            void reset();

            void handleError();

            PyObjectPtr &operator=(PyObjectPtr &&other);

            explicit operator bool() const;
            operator PyObject *() const;
            PyObject **operator&();

            static PyObjectPtr None();

        protected:
            PyObject *mObject = nullptr;
        };

        struct MADGINE_PYTHON3_EXPORT PyObjectFieldAccessor {
            PyObjectFieldAccessor &operator=(const PyObjectPtr &value);

            PyObjectPtr mObject;
            PyObjectPtr mKey;
        };

    }
}
}