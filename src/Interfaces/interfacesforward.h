#pragma once

namespace Engine {

struct InStream;
struct OutStream;

    enum StreamError {
    NO_ERROR = 0,
    WOULD_BLOCK,
    ALREADY_CONNECTED,
    TIMEOUT,
    NO_SERVER,
    NO_CONNECTION,
    CONNECTION_REFUSED,
    UNKNOWN_ERROR
};

namespace Debug {

    struct TraceBack;
    struct StackTraceIterator;

    namespace Memory {
        struct StatsMemoryResource;
        struct MemoryTracker;
    }

}

namespace Window {
    struct WindowEventListener;
    struct Window;
}

namespace Filesystem {
    struct Path;
    struct FileQuery;
    struct FileQueryState;

    struct SharedLibraryQuery;
    struct SharedLibraryQueryState;
}

}

