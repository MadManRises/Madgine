#include "../python3lib.h"

#include "pymoduleptr.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Scripting::Python3::PyModulePtr)
METATABLE_END(Engine::Scripting::Python3::PyModulePtr)

namespace Engine {
namespace Scripting {
    namespace Python3 {
 
        PyModulePtr::PyModulePtr(const std::string_view &name)
        {
            PyObject *modules = PyImport_GetModuleDict();
            mObject = PyDict_GetItemString(modules, name.data());
            if (mObject) {
                Py_INCREF(mObject);
            } else {
                mObject = PyImport_ImportModule(name.data());
            }

        }

    }
}
}