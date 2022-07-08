#include "../python3lib.h"

#include "pydictptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        PyDictPtr::PyDictPtr(PyObject *dict)
            : PyObjectPtr(dict)
        {
            assert(PyDict_Check(dict));
        }

        PyDictPtr::PyDictPtr(const std::map<std::string, PyObjectPtr> &values)
            : PyObjectPtr(PyDict_New())
        {
            for (auto &[key, value] : values) {
                auto result = PyDict_SetItemString(mObject, key.c_str(), value);
                assert(result == 0);
            }
        }

        PyDictPtr::iterator::iterator(PyObject *dict)
            : mDict(dict)
        {
            mDone = !PyDict_Next(mDict, &mPos, &mKey, &mValue);
        }

        std::pair<PyObject *, PyObject *> PyDictPtr::iterator::operator*() const
        {
            return { mKey, mValue };
        }

        void PyDictPtr::iterator::operator++()
        {
            mDone = !PyDict_Next(mDict, &mPos, &mKey, &mValue);
            if (mDone)
                mPos = 0;
        }

        bool PyDictPtr::iterator::operator==(const iterator &other) const
        {
            assert(mDict == other.mDict || !mDict || !other.mDict);
            if (mDone != other.mDone)
                return false;
            return mPos == other.mPos;
        }

        PyDictPtr::iterator PyDictPtr::begin()
        {
            return { mObject };
        }

        PyDictPtr::iterator PyDictPtr::end()
        {
            return {};
        }

        size_t PyDictPtr::size() const
        {
            return PyDict_Size(mObject);
        }

    }
}
}