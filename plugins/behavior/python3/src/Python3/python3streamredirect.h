#pragma once

#include "Generic/closure.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct Python3StreamRedirect {

            Python3StreamRedirect(Closure<void(std::string_view)> out = {});
            Python3StreamRedirect(const Python3StreamRedirect &) = delete;
            ~Python3StreamRedirect();

            void redirect(std::string_view name);
            void reset(std::string_view name);

            int write(std::string_view text);

            void setOut(Closure<void(std::string_view)> out);
            Closure<void(std::string_view)> out();

        private:
            Closure<void(std::string_view)> mOut;
            std::map<std::string_view, PyObject *> mOldStreams;
        };

    }
}
}