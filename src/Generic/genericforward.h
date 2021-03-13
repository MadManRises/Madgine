#pragma once

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

struct OffsetPtrTag;
template <typename T, size_t>
struct TaggedPlaceholder;

template <typename T>
struct ControlBlock;



struct Any;

struct ByteBuffer;

struct CoWString;

}