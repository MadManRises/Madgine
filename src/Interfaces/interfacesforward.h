#pragma once

namespace Engine {

struct InterfacesVector {
    int x;
    int y;

    InterfacesVector operator-(const InterfacesVector& other) const {
        return { x - other.x, y - other.y };
    }
    InterfacesVector operator+(const InterfacesVector &other) const
    {
        return { x + other.x, y + other.y };
    }
};

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
    struct OSWindow;
    struct WindowSettings;
}

namespace Filesystem {
    struct Path;
    struct FileQuery;
    struct FileQueryState;
}

namespace Dl {
    struct SharedLibraryQuery;
    struct SharedLibraryQueryState;
}

namespace Input {
    struct KeyEventArgs;
    struct PointerEventArgs;

    namespace MouseButton {
        enum MouseButton : unsigned char;
    }

    namespace Key {
        enum Key : uint8_t;
    }
}

namespace Util {

    struct Log;

    struct LogListener;
    struct StandardLog;
}

}

