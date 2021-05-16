#pragma once

namespace Engine {
namespace Scripting {
    namespace Python3 {


        struct MADGINE_PYTHON3_EXPORT PyObjectPtr {
        
            PyObjectPtr() = default;
            PyObjectPtr(PyObject *object);
            PyObjectPtr(PyObjectPtr &&other);
            ~PyObjectPtr();

            PyObjectPtr get(const std::string_view &name) const;
            PyObjectPtr call(const std::string_view &name, const char *format, ...) const;
            PyObjectPtr call(const std::string_view &name, const PyObjectPtr &args) const;

            void reset();

            PyObjectPtr &operator=(PyObjectPtr &&other);

            explicit operator bool() const;
            operator PyObject *() const;

        protected:
            PyObject *mObject = nullptr;
        };

    }
}
}