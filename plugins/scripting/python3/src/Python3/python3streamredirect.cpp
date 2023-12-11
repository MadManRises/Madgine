#include "python3lib.h"

#include "python3streamredirect.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "util/pyobjectutil.h"

METATABLE_BEGIN(Engine::Scripting::Python3::Python3StreamRedirect)
FUNCTION(write, text)
METATABLE_END(Engine::Scripting::Python3::Python3StreamRedirect)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3StreamRedirect::Python3StreamRedirect(Lambda<void(std::string_view)> out)
            : mOut(std::move(out))
        {
        }

        Python3StreamRedirect::~Python3StreamRedirect()
        {
            while (!mOldStreams.empty()) {
                reset(mOldStreams.begin()->first);
            }
        }

        void Python3StreamRedirect::redirect(std::string_view name)
        {
            if (!mOldStreams[name]) {
                mOldStreams[name] = PySys_GetObject(name.data()); // borrowed
            }

            PySys_SetObject(name.data(), Scripting::Python3::toPyObject(TypedScopePtr { this }));
        }

        void Python3StreamRedirect::reset(std::string_view name)
        {
            auto it = mOldStreams.find(name);
            if (it != mOldStreams.end()) {
                Py_DECREF(PySys_GetObject(name.data()));
                PySys_SetObject(name.data(), it->second);
                mOldStreams.erase(it);
            }
        }

        int Python3StreamRedirect::write(std::string_view text)
        {
            if (mOut) {
                mOut(text);
                return text.size();
            } else {
                LOG(text);
                return text.size();
            }
        }

        void Python3StreamRedirect::setOut(Lambda<void(std::string_view)> out)
        {
            mOut = std::move(out);
        }

        Lambda<void(std::string_view)> Python3StreamRedirect::out() 
        {
            return std::move(mOut);
        }

    }
}
}
