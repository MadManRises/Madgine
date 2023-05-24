#pragma once


#include "util/pyexecution.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct Python3Debugger {

            void setup();

            static int trace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg);
            
        private:
        };

    }
}
}