#pragma once

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3Lock {
            Python3Lock(std::streambuf *o = nullptr);
            Python3Lock(const Python3Lock &) = delete;
            Python3Lock(Python3Lock &&);
            ~Python3Lock();

        private:
            std::optional<PyGILState_STATE> mState;
        };

    }
}
}