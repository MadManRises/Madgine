#pragma once

#if ENABLE_THREADING
#    if USE_CUSTOM_THREADLOCAL
#        include "global.h"
#        include "threadstorage.h"

#        define THREADLOCAL(T) ::Engine::Threading::Global<T, ::Engine::Threading::ThreadStorage>
#    else
#        include "../generic/proxy.h"

#        define THREADLOCAL(T) thread_local ::Engine::Proxy<T>
#    endif
#else
#        include "../generic/proxy.h"

#    define THREADLOCAL(T) ::Engine::Proxy<T>
#endif

#if EMSCRIPTEN
#    define thread_local thread_local_not_supported_on_emscripten provoke_syntax_error
#endif
