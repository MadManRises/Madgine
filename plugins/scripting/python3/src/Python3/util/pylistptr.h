#pragma once

#include "pyobjectptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {
    
        struct MADGINE_PYTHON3_EXPORT PyListPtr : PyObjectPtr {
            PyListPtr() = default;
            PyListPtr(PyObject *list);
            PyListPtr(const std::vector<PyObjectPtr> &values);

            using PyObjectPtr::operator=;

            struct iterator {
                iterator() = default;
                iterator(PyObject *list);

                PyObject *operator*() const;
                void operator++();
                bool operator==(const iterator &) const;

            private:
                PyObject *mList = nullptr;
                Py_ssize_t mIndex = 0;
            };

            iterator begin();
            iterator end();

            size_t size() const;
        };
    
    }
}
}