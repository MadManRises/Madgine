#pragma once

/**
 * @brief Main namespace for the Madgine
*/
namespace Engine {

template <typename C, typename Base>
struct container_api_impl;

template <typename C>
struct GenerationContainer;

template <typename T>
using GenerationVector = GenerationContainer<std::vector<T>>;

template <typename...>
struct type_pack;

struct CompoundAtomicOperation;

struct MemberOffsetPtrTag;
template <typename T, size_t>
struct TaggedPlaceholder;

struct Any;

template <typename>
struct ByteBufferImpl;
using ByteBuffer = ByteBufferImpl<const void>;
using WritableByteBuffer = ByteBufferImpl<void>;

struct CoWString;
template <typename T>
struct CoW;

template <typename>
struct EnumType;
template <typename, typename>
struct BaseEnum;
struct EnumMetaTable;

template <typename>
struct Generator;
template <typename>
struct CoroutineHandle;

struct OffsetPtr;

struct Stream;

enum class AccessMode {
    READ,
    WRITE
};

namespace Execution {
    template <typename, typename, typename>
    struct Sender;
}

}