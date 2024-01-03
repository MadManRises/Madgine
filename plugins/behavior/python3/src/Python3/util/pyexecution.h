#pragma once

#include "Generic/genericresult.h"

#include "Generic/execution/concepts.h"

#include "pyframeptr.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {        
        void setupExecution();

        bool stackUnwindable();

        PyObject *evalFrame(PyFrameObject *frame, int throwExc);
        void evalFrame(KeyValueReceiver &receiver, PyFramePtr frame);
        void evalFrames(KeyValueReceiver &receiver, std::vector<PyFramePtr> frames);

        PyObject *suspend(Lambda<void(KeyValueReceiver &, std::vector<PyFramePtr>, Lambda<void(std::string_view)>)> callback);

    }
}
}