#pragma once

namespace Engine {

struct ValueType;
struct ValueTypeRef;
struct KeyValuePair;

struct ScopeBase;
struct MetaTable;
struct ScopeIterator;
struct Accessor;
struct ScopeField;
struct TypedScopePtr;
struct TypeInfo;
struct ApiFunction;
struct BoundApiFunction;
struct FunctionTable;
using ArgumentList = std::vector<ValueType>;

struct ObjectInstance;
struct ObjectPtr;
struct ObjectFieldAccessor;

template <typename RefT>
struct VirtualIterator;

enum KeyValueValueFlags : uint8_t;

struct KeyValueVirtualIteratorBase;
using KeyValueVirtualIterator = VirtualIterator<KeyValuePair>;

struct MadgineObject;

struct IndexHolder;

struct UniqueComponentCollectorManager;
template <typename T, typename _Collector, typename _Base = typename _Collector::Base>
struct UniqueComponent;
template <typename T, typename _Collector, typename Base = typename _Collector::Base>
struct VirtualUniqueComponentBase;

struct ComponentRegistryBase;

struct Any;

struct ByteBuffer;

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
        struct Profiler;
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
    struct SerializableUnitBase;
    struct BufferedInOutStream;
    struct BufferedInStream;
    struct BufferedOutStream;
    struct TopLevelSerializableUnitBase;
    struct SyncableBase;
    struct SerializableBase;
    struct FileBuffer;
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
    struct NetworkManager;
}

namespace Plugins {
    struct PluginManager;
    struct Plugin;
    struct PluginSection;
    struct PluginListener;

    struct BinaryInfo;
}

namespace Util {

    struct Log;

    struct LogListener;

    struct Process;
    struct StandardLog;
}

namespace Threading {
    struct ConnectionBase;
    template <typename T, typename... _Ty>
    struct ConnectionInstance;
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

namespace Render {
    typedef uintptr_t TextureHandle;
    typedef int RenderPassFlags;

    struct Font;
    struct Glyph;
}

namespace Resources {
    struct ResourceManager;
    template <typename Data, typename Container, typename Storage>
    struct Resource;
    template <typename T, typename Data, typename Container, typename Storage>
    struct ResourceLoader;
    template <typename T, typename _Data, typename Container, typename Storage, typename Base>
    struct ResourceLoaderImpl;
    struct ResourceBase;
}

struct Vector2;
struct Vector3;
struct Vector4;

struct Vector2i;
struct Vector3i;

struct Rect2i;

struct Matrix3;
struct Matrix4;

struct Quaternion;


struct Ray;
struct Sphere;
struct Plane;
struct AABB;
struct BoundingBox;
}
