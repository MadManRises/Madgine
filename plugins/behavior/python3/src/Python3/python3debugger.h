#pragma once


#include "Madgine/debug/debugger.h"

#include "util/pyexecution.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3DebugLocation : Debug::DebugLocation {

            Python3DebugLocation(PyFrameObject *frame);

            virtual std::string toString() const override;
            virtual std::map<std::string_view, ValueType> localVariables() const override;

            Filesystem::Path file() const;
            std::string module() const;
            size_t lineNr() const;

            PyFrameObject *mFrame;
        };


        struct Python3Debugger {

            void setup();

            static int trace(PyObject *obj, PyFrameObject *frame, int event, PyObject *arg);
            
        private:
        };

    }
}
}