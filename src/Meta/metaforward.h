#pragma once

namespace Engine {

struct ValueType;
struct ValueTypeRef;
struct KeyValuePair;

template <typename T, typename Base>
struct VirtualScope;
template <typename Base>
struct VirtualScopeBase;
struct ProxyScopeBase;
struct MetaTable;
struct ScopeIterator;
struct Accessor;
struct ScopeField;
struct TypedScopePtr;
struct OwnedScopePtr;
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
template <typename RefT>
struct VirtualRange;

enum KeyValueValueFlags : uint8_t;

using KeyValueVirtualIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualRange = VirtualRange<KeyValuePair>;

template <typename T>
struct MadgineObject;

struct IndexHolder;


struct ComponentRegistryBase;


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
    struct SerializableDataUnit;
    struct SerializableUnitBase;
    struct SyncableUnitBase;
    struct BufferedInOutStream;
    struct BufferedInStream;
    struct BufferedOutStream;
    struct TopLevelUnitBase;
    struct SyncableBase;
    struct SerializableBase;
    struct FileBuffer;
    struct MessageHeader;
    struct SerializeManager;
    struct SyncManager;
    struct SerializeStreambuf;

    struct SerializableUnitPtr;
    struct SerializableUnitConstPtr;
    struct SerializableDataPtr;
    struct SerializableDataConstPtr;

	struct Serializer;

    struct Formatter;

    struct CompareStreamId;

    struct PendingRequest;

    typedef uint32_t ParticipantId;
    typedef uint32_t TransactionId;
    typedef uint32_t UnitId;
    enum class UnitIdTag {
        NONE = 0,
        SYNCABLE = 1,
        SERIALIZABLE = 2
    };

    struct buffered_streambuf;

    struct noparent_deleter;

    struct SerializeTable;

    
    using SyncableUnitMap = std::map<UnitId, SyncableUnitBase *>;
    using SerializableUnitMap = std::map<const SerializableDataUnit *, uint32_t>;
    using SerializableUnitList = std::vector<SerializableDataUnit*>;

    struct SerializableMapHolder;
    struct SerializableListHolder;

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
    template <typename Loader>
    struct ResourceType;
    template <typename T, typename Data, typename Container, typename Storage>
    struct ResourceLoader;
    template <typename T, typename _Data, typename Base>
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
