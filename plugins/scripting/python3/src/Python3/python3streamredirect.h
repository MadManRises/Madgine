#pragma once

#include "Generic/lambda.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct Python3StreamRedirect {

            Python3StreamRedirect(Lambda<void(std::string_view)> out = {});
            Python3StreamRedirect(const Python3StreamRedirect &) = delete;
            ~Python3StreamRedirect();

            void redirect(std::string_view name);
            void reset(std::string_view name);

            int write(std::string_view text);

            void setOut(Lambda<void(std::string_view)> out);
            Lambda<void(std::string_view)> out();

        private:
            Lambda<void(std::string_view)> mOut;
            std::map<std::string_view, PyObject *> mOldStreams;
        };

    }
}
}