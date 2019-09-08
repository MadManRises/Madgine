#pragma once

namespace Engine {
struct ValueType;

struct InvScopePtr;

enum KeyValueValueFlags : uint8_t;

class KeyValueVirtualIteratorBase;

struct ScopeBase;
struct MetaTable;
struct ScopeIterator;
struct Accessor;
struct TypedScopePtr;

struct ObjectInstance;
struct ObjectPtr;
struct ObjectFieldAccessor;

struct IndexHolder;

struct UniqueComponentCollectorManager;
struct ComponentRegistryBase;

struct InStream;
struct OutStream;

template <typename RefT>
struct VirtualIterator;

using KeyValueVirtualIterator = VirtualIterator<std::pair<ValueType, ValueType>>;

using ArgumentList = std::vector<ValueType>;

struct ApiMethod;

namespace CLI {
    struct CLICore;
}

namespace Debug {

    struct TraceBack;
    struct StackTraceIterator;

    namespace Memory {
        struct StatsMemoryResource;
        struct MemoryTracker;
    }

    namespace Profiler {
        class Profiler;
    }
}

namespace Ini {
    struct IniFile;
}

namespace Serialize {
    struct SerializeInStream;
    struct SerializeOutStream;
    class SerializableUnitBase;
    struct BufferedInOutStream;
    struct BufferedInStream;
    struct BufferedOutStream;
    class TopLevelSerializableUnitBase;
    struct SyncableBase;
    struct SerializableBase;
    class FileBuffer;
    struct MessageHeader;
    struct SerializeManager;
    struct SerializeStreambuf;

	struct Formatter;

    class EOLType;

    struct CompareStreamId;

    typedef size_t ParticipantId;

    struct buffered_streambuf;

    struct noparent_deleter;

	struct SerializeTable;
    using SerializableUnitMap = std::map<size_t, SerializableUnitBase *>;

    enum MessageType {
        STATE,
        ACTION,
        REQUEST
    };

    enum Command {
        INITIAL_STATE_DONE,
        STREAM_EOF
    };
}

namespace Network {
    class NetworkManager;
}

namespace Plugins {
    class PluginManager;
    struct Plugin;
    class PluginSection;
    struct PluginListener;

    struct BinaryInfo;
}

namespace Util {

    class Log;

    class LogListener;

    class Process;
    class StandardLog;
}

namespace SignalSlot {
    class ConnectionBase;
    template <class T, class... _Ty>
    class ConnectionInstance;
    struct TaskQueue;
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

namespace Threading {
    struct WorkGroup;
    struct WorkGroupHandle;
    struct Scheduler;

    class FrameListener;
    struct FrameLoop;

    struct DataMutex;
}

class Vector2;
class Vector3;

struct Vector2i;

class Matrix3;
class Matrix4;

struct TypeInfo;

struct Ray;
struct Sphere;
struct Plane;
struct AABB;
struct BoundingBox;
}

struct lua_State;
struct luaL_Reg;
