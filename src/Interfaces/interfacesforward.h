#pragma once

namespace Engine {

struct InStream;
struct OutStream;

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
    struct WindowSettings;
}

namespace Filesystem {
    struct Path;
    struct FileQuery;
    struct FileQueryState;

    struct SharedLibraryQuery;
    struct SharedLibraryQueryState;
}

}

