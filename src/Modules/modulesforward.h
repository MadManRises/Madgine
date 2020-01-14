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
template <typename T, typename _Collector, typename _Base = typename _Collector::Base>
struct UniqueComponent;
template <class T, class _Collector, typename Base = typename _Collector::Base>
class VirtualUniqueComponentBase;

struct ComponentRegistryBase;

struct InStream;
struct OutStream;

template <typename RefT>
struct VirtualIterator;

using KeyValueVirtualIterator = VirtualIterator<std::pair<ValueType, ValueType>>;

using ArgumentList = std::vector<ValueType>;

struct ApiMethod;

struct Any;

namespace CLI {
    struct CLICore;
    struct ParameterBase;
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
        struct ProfilerThread;
    }
}

namespace Ini {
    struct IniFile;
    struct IniSection;
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
    struct SyncManager;
    struct SerializeStreambuf;

	struct Serializer;

    struct Formatter;

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
    struct PluginManager;
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

namespace Threading {
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

    struct FrameListener;
    struct FrameLoop;

    struct DataMutex;
}

namespace Font {

    struct Font;
    struct Glyph;

}

namespace Render {
    /*struct Vertex;
    struct Vertex2;
    struct Vertex3;*/
    typedef int RenderPassFlags;
}

namespace Resources {
    struct ResourceManager;
    template <typename Data, typename Container, typename Storage>
    class Resource;
    template <class T, class Data, typename Container, typename Storage>
    struct ResourceLoader;
    template <typename T, class _Data, typename Container, typename Storage, typename Base>
    struct ResourceLoaderImpl;
    struct ResourceBase;
}

class Vector2;
class Vector3;

struct Vector2i;
struct Vector3i;

struct Rect2i;

class Matrix3;
class Matrix4;

struct TypeInfo;

struct Ray;
struct Sphere;
struct Plane;
struct AABB;
struct BoundingBox;

class MadgineObject;
}
