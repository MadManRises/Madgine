#pragma once

#include "Generic/enum.h"

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
struct KeyValueFunction;
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
    

namespace Serialize {
    struct SerializeStream;
    struct SerializableDataUnit;
    struct SerializableUnitBase;
    struct SyncableUnitBase;
    struct TopLevelUnitBase;
    struct SyncableBase;
    struct SerializableBase;
    struct FileBuffer;
    struct MessageHeader;
    struct SerializeManager;
    struct SyncManager;
    struct SerializeStreamData;
    struct SyncStreamData;
    struct FormattedSerializeStream;
    struct FormattedBufferedStream;

    struct StreamResult;

    struct SerializableUnitPtr;
    struct SerializableUnitConstPtr;
    struct SerializableDataPtr;
    struct SerializableDataConstPtr;

	struct Serializer;
    struct SerializeTableCallbacks;

    struct Formatter;

    struct CompareStreamId;

    struct PendingRequest;

    struct CreatorCategory;

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

    ENUM(MessageType,
        STATE,
        ACTION,
        REQUEST
    );

    enum Command {
        INITIAL_STATE_DONE
    };

    template <typename, typename... Configs>
    struct Operations;

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
