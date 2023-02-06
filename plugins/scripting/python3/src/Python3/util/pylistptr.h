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

            using value_type = PyObject*;

            struct iterator {
                using value_type = PyObject*;
                using difference_type = ptrdiff_t;

                iterator() = default;
                iterator(PyObject *list);

                PyObject *operator*() const;
                iterator &operator++();
                iterator operator++(int);
                bool operator==(const iterator &) const;

            private:
                PyObject *mList = nullptr;
                Py_ssize_t mIndex = 0;
            };

            iterator begin() const;
            iterator end() const;

            size_t size() const;
        };
    
    }
}
}