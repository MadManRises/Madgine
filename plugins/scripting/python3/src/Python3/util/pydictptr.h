#pragma once

#include "pyobjectptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT PyDictPtr : PyObjectPtr {
            PyDictPtr() = default;
            PyDictPtr(PyObject *dict);
            PyDictPtr(const std::map<std::string, PyObjectPtr> &values);

            using PyObjectPtr::operator=;

            struct iterator {
                iterator() = default;
                iterator(PyObject *dict);

                std::pair<PyObject *, PyObject *> operator*() const;
                void operator++();
                bool operator==(const iterator &) const;

            private:
                PyObject *mDict = nullptr;
                Py_ssize_t mPos = 0;
                bool mDone = true;
                PyObject *mKey = nullptr;
                PyObject *mValue = nullptr;
            };

            iterator begin();
            iterator end();

            size_t size() const;
        };

    }
}
}