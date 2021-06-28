#include "../python3lib.h"

#include "pydictptr.h"


namespace Engine {
namespace Scripting {
    namespace Python3 {
 
        PyDictPtr::PyDictPtr(const std::map<std::string, PyObjectPtr> &values) : PyObjectPtr(PyDict_New())
        {
            for (auto& [key, value] : values) {
                auto result = PyDict_SetItemString(mObject, key.c_str(), value);
                assert(result == 0);
            }

        }

    }
}
}