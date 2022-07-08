#include "../python3lib.h"

#include "pylistptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        PyListPtr::PyListPtr(PyObject *dict)
            : PyObjectPtr(dict)
        {
            assert(PyList_Check(dict));
        }

        PyListPtr::PyListPtr(const std::vector<PyObjectPtr> &values)
            : PyObjectPtr(PyList_New(values.size()))
        {
            size_t i = 0;
            for (auto &value : values) {
                auto result = PyList_SetItem(mObject, i++, value);
                assert(result == 0);
            }
        }

        PyListPtr::iterator::iterator(PyObject *list)
            : mList(PyList_Size(list) == 0 ? nullptr : list)
        {
        }

        PyObject *PyListPtr::iterator::operator*() const
        {
            return PyList_GetItem(mList, mIndex);
        }

        void PyListPtr::iterator::operator++()
        {
            assert(mList);
            ++mIndex;
            if (mIndex == PyList_Size(mList)) {
                mIndex = 0;
                mList = nullptr;
            }
        }

        bool PyListPtr::iterator::operator==(const iterator &other) const
        {
            assert(mList == other.mList || !mList || !other.mList);
            if (mList != other.mList)
                return false;
            return mIndex == other.mIndex;
        }

        PyListPtr::iterator PyListPtr::begin()
        {
            return { mObject };
        }

        PyListPtr::iterator PyListPtr::end()
        {
            return {};
        }

        size_t PyListPtr::size() const
        {
            return PyList_Size(mObject);
        }

    }
}
}