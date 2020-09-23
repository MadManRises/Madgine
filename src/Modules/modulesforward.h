#pragma once

namespace Engine {

struct ValueType;
struct ValueTypeRef;
struct KeyValuePair;

struct ScopeBase;
template <typename T, typename Base>
struct VirtualScope;
template <typename Base>
struct VirtualScopeBase;
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

	
template <typename C, typename Base>
struct container_api_impl;

struct ObjectInstance;
struct ObjectPtr;
struct ObjectFieldAccessor;

template <typename RefT>
struct VirtualIterator;
template <typename RefT>
struct VirtualRange;

enum KeyValueValueFlags : uint8_t;

using KeyValueVirtualIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualRange = VirtualRange<KeyValuePair>;

template <typename T>
struct MadgineObject;

struct IndexHolder;

struct UniqueComponentCollectorManager;
template <typename T, typename _Collector, typename _Base>
struct UniqueComponent;
template <typename Observer, typename Registry, typename _Base, typename... _Ty>
struct UniqueComponentSelector;
template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct UniqueComponentContainer;
template <typename T, typename _Collector, typename Base>
struct VirtualUniqueComponentBase;
template <typename T, typename Base, typename _VBase = Base>
struct VirtualUniqueComponentImpl;
template <typename _Base, typename... _Ty>
struct UniqueComponentRegistry;
template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentCollector;


template <typename T>
struct GenerationVector;

struct ComponentRegistryBase;

struct Any;

struct ByteBuffer;

struct CoWString;

struct CompoundAtomicOperation;

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

    struct PendingRequest;

    typedef uint32_t ParticipantId;
    typedef uint32_t TransactionId;
    typedef uint32_t UnitId;

    struct buffered_streambuf;

    struct noparent_deleter;

    struct SerializeTable;

    
    using SerializableUnitMap = std::map<UnitId, SerializableUnitBase *>;

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

namespace Threading {
    struct ConnectionBase;
    template <typename T, typename... _Ty>
    struct ConnectionInstance;
    struct TaskQueue;
    struct TaskHandle;
    struct Barrier;
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
    struct ResourceLoaderBase;
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
