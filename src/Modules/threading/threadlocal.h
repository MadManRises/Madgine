#pragma once

#include "../generic/proxy.h"

#include "threadstorage.h"
#include "global.h"

#if ENABLE_THREADING
#    if USE_CUSTOM_THREADLOCAL
#        define THREADLOCAL(T) ::Engine::Threading::Global<T, ::Engine::Threading::ThreadStorage>
#    else
#        define THREADLOCAL(T) thread_local ::Engine::Proxy<T>
#    endif
#else
#    define THREADLOCAL(T) ::Engine::Proxy<T>
#endif

#if EMSCRIPTEN
#    define thread_local thread_local_not_supported_on_emscripten provoke_syntax_error
#endif
