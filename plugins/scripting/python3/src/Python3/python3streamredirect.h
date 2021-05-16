#pragma once

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct Python3StreamRedirect {

            Python3StreamRedirect(std::streambuf *buf);
            Python3StreamRedirect(const Python3StreamRedirect &) = delete;
            ~Python3StreamRedirect();

            void redirect(const std::string_view &name);
            void reset(const std::string_view &name);

            int write(const std::string_view &text);

            void setBuf(std::streambuf *buf);
            std::streambuf *buf() const;

        private:
            std::streambuf *mBuf;
            std::map<std::string_view, PyObject *> mOldStreams;
        };

    }
}
}