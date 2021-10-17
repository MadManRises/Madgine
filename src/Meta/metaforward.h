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
struct FunctionArgument;
struct Function;
struct EnumHolder;
using ArgumentList = std::vector<ValueType>;

struct ExtendedValueTypeDesc;
struct ValueTypeDesc;
	

struct ObjectInstance;
struct ObjectPtr;

template <typename RefT>
struct VirtualIterator;
template <typename RefT, typename AssignDefault = DefaultAssign>
struct VirtualRange;

enum KeyValueValueFlags : uint8_t;

template <bool reference_to_ptr>
struct Functor_to_KeyValuePair;
template <bool reference_to_ptr>
struct Functor_to_ValueTypeRef;

using KeyValueVirtualAssociativeIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualAssociativeRange = VirtualRange<KeyValuePair, Functor_to_KeyValuePair<true>>;
using KeyValueVirtualSequenceIterator = VirtualIterator<ValueTypeRef>;
using KeyValueVirtualSequenceRange = VirtualRange<ValueTypeRef, Functor_to_ValueTypeRef<true>>;

template <typename T>
struct MadgineObject;



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
    struct SerializeStreamData;
    struct BufferedStreamData;

    struct StreamResult;

    struct SerializableUnitPtr;
    struct SerializableUnitConstPtr;
    struct SerializableDataPtr;
    struct SerializableDataConstPtr;

	struct Serializer;

    struct Formatter;

    struct CompareStreamId;

    struct PendingRequest;

    typedef int StateTransmissionFlags;

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
        INITIAL_STATE_DONE
    };

    template <typename, typename... Configs>
    struct Operations;

        template <typename T, typename... Configs, typename Hierarchy = std::monostate>
    StreamResult read(SerializeInStream &in, T &t, const char *name, const Hierarchy &hierarchy = {}, StateTransmissionFlags flags = 0);

    template <typename T, typename... Configs, typename Hierarchy = std::monostate>
    void write(SerializeOutStream &out, const T &t, const char *name, const Hierarchy &hierarchy = {}, StateTransmissionFlags flags = 0);
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

struct NormalizedVector3;

struct Vector2i;
struct Vector3i;
struct Vector4i;

struct Rect2i;

struct Matrix3;
struct Matrix4;

struct Quaternion;

struct Frustum;
struct Ray;
struct Sphere;
struct Plane;
struct AABB;
struct BoundingBox;
}
