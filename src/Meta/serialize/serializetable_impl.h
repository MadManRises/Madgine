#pragma once

#include "hierarchy/serializer.h"
#include "hierarchy/syncfunction.h"

#include "container/unithelper.h"

#include "Generic/memberoffsetptr.h"

#include "hierarchy/serializetable.h"

#include "operations.h"

#include "container/container_operations.h"

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    void writeFunctionAction(SyncableUnitBase *unit, uint16_t index, const void *args, const std::set<ParticipantId> &targets, ParticipantId answerTarget, MessageId answerId);
    void writeFunctionResult(SyncableUnitBase *unit, uint16_t index, const void *result, ParticipantId answerTarget, MessageId answerId);
    void writeFunctionRequest(SyncableUnitBase *unit, uint16_t index, FunctionType type, const void *args, ParticipantId requester, MessageId requesterTransactionId, GenericMessagePromise promise = {});

    namespace __serialize_impl__ {

        struct SerializerTag;
        struct FunctionTag;
        template <size_t... Ids>
        using FunctionLineStruct = LineStruct<FunctionTag, Ids...>;
        template <size_t... Ids>
        using SerializerLineStruct = LineStruct<SerializerTag, Ids...>;

        template <typename _disambiguate__dont_remove, auto P, auto Getter, auto Setter>
        constexpr Serializer encapsulated_pointer(const char *name)
        {
            using traits = CallableTraits<decltype(P)>;
            using Unit = typename traits::class_type;
            using T = std::decay_t<typename traits::return_type>;

            using getter_traits = CallableTraits<decltype(Getter)>;
            static_assert(std::same_as<Unit, std::remove_const_t<typename getter_traits::class_type>>);
            static_assert(std::same_as<T, typename getter_traits::return_type>);

            using setter_traits = CallableTraits<decltype(Setter)>;
            static_assert(std::same_as<Unit, typename setter_traits::class_type>);

            //TODO remove const in tuple types
            static_assert(std::same_as<typename setter_traits::argument_types, type_pack<T>>);

            return {
                name,
                []() {
                    return OffsetPtr {};
                },
                [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                    const Unit *unit = static_cast<const Unit *>(_unit);
                    write(out, (unit->*Getter)(), name, CallerHierarchyPtr { hierarchy.append(unit) });
                },
                [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                    Unit *unit = static_cast<Unit *>(_unit);
                    (unit->*Setter)(nullptr);
                    return read(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest *request) -> StreamResult {
                    throw "Unsupported";
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &inout, MessageId id) -> StreamResult {
                    throw "Unsupported";
                },
                [](SerializableDataUnit *_unit, FormattedSerializeStream &in, bool success) -> StreamResult {
                    Unit *unit = static_cast<Unit *>(_unit);
                    STREAM_PROPAGATE_ERROR(applyMap(in, unit->*P, success));
                    return {};
                },
                [](SerializableDataUnit *unit, bool b) {
                },
                [](SerializableDataUnit *_unit, bool active, bool existenceChanged) {
                    Unit *unit = static_cast<Unit *>(_unit);
                    if (active) {
                        T val = unit->*P;
                        unit->*P = nullptr;
                        (unit->*Setter)(val);
                    } else {
                        T val = unit->*P;
                        (unit->*Setter)(nullptr);
                        unit->*P = val;
                    }
                },
                [](SerializableDataUnit *unit) {
                },
                [](const SyncableUnitBase *unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                    throw "Unsupported";
                },
                [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
                    throw "Unsupported";
                }
            };
        }

        template <typename _disambiguate__dont_remove, auto Getter, auto Setter>
        constexpr Serializer encapsulated_field(const char *name)
        {

            using getter_traits = CallableTraits<decltype(Getter)>;
            using Unit = std::decay_t<typename getter_traits::class_type>;
            using T = std::decay_t<typename getter_traits::return_type>;

            using setter_traits = CallableTraits<decltype(Setter)>;
            static_assert(std::same_as<Unit, std::decay_t<typename setter_traits::class_type>>);

            static_assert(std::same_as<typename setter_traits::decay_argument_types, type_pack<T>>);

            return {
                name,
                []() {
                    return OffsetPtr {};
                },
                [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                    const Unit *unit = static_cast<const Unit *>(_unit);
                    write(out, (unit->*Getter)(), name, hierarchy.append(unit));
                },
                [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                    Unit *unit = static_cast<Unit *>(_unit);
                    MakeOwning_t<T> dummy;
                    STREAM_PROPAGATE_ERROR(read(in, dummy, name, CallerHierarchyPtr { hierarchy.append(unit) }));
                    (unit->*Setter)(std::move(dummy));
                    return {};
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest &request) -> StreamResult {
                    throw "Unsupported";
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &inout, MessageId id) -> StreamResult {
                    throw "Unsupported";
                },
                [](SerializableDataUnit *_unit, FormattedSerializeStream &in, bool success) {
                    return StreamResult {};
                },
                [](SerializableDataUnit *unit, bool b) {
                },
                [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                },
                [](SerializableDataUnit *unit) {
                },
                [](const SyncableUnitBase *unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                    throw "Unsupported";
                },
                [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
                    throw "Unsupported";
                }
            };
        }

        template <typename _disambiguate__dont_remove, auto P, typename... Configs>
        constexpr Serializer field(const char *name)
        {
            using traits = CallableTraits<decltype(P)>;
            using Unit = typename traits::class_type;
            using T = std::decay_t<typename traits::return_type>;

            return {
                name,
                []() {
                    return MemberOffsetPtr<Unit, T> { P }.template offset<SerializableDataUnit>();
                },
                [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                    const Unit *unit = static_cast<const Unit *>(_unit);
                    write<T, Configs...>(out, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
                },
                [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                    Unit *unit = static_cast<Unit *>(_unit);
                    return read<T, Configs...>(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
                },
                [](SyncableUnitBase *_unit, FormattedBufferedStream &in, PendingRequest &request) -> StreamResult {
                    if constexpr (std::derived_from<T, SyncableBase>) {
                        Unit *unit = static_cast<Unit *>(_unit);
                        return readAction<T, Configs...>(unit->*P, in, request, CallerHierarchyPtr { CallerHierarchy { unit } });
                    } else
                        throw "Unsupported";
                },
                [](SyncableUnitBase *_unit, FormattedBufferedStream &inout, MessageId id) -> StreamResult {
                    if constexpr (std::derived_from<T, SyncableBase>) {
                        Unit *unit = static_cast<Unit *>(_unit);
                        return readRequest<T, Configs...>(unit->*P, inout, id, CallerHierarchyPtr { CallerHierarchy { unit } });
                    } else
                        throw "Unsupported";
                },
                [](SerializableDataUnit *unit, FormattedSerializeStream &in, bool success) {
                    return applyMap<T, Configs...>(in, static_cast<Unit *>(unit)->*P, success);
                },
                [](SerializableDataUnit *unit, bool b) {
                    setSynced<T, Configs...>(static_cast<Unit *>(unit)->*P, b);
                },
                [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                    setActive<T, Configs...>(static_cast<Unit *>(unit)->*P, active, existenceChanged);
                },
                [](SerializableDataUnit *unit) {
                    if constexpr (std::derived_from<T, SerializableUnitBase>)
                        setParent<T, Configs...>(static_cast<Unit *>(unit)->*P, unit);
                },
                [](const SyncableUnitBase *_unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                    if constexpr (std::derived_from<T, SyncableBase>) {
                        const Unit *unit = static_cast<const Unit *>(_unit);
                        writeAction<T, Configs...>(unit->*P, outStreams, data, CallerHierarchyPtr { CallerHierarchy { unit } });
                    } else
                        throw "Unsupported";
                },
                [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
                    if constexpr (std::derived_from<T, SyncableBase>) {
                        const Unit *unit = static_cast<const Unit *>(_unit);
                        writeRequest<T, Configs...>(unit->*P, out, data, CallerHierarchyPtr { CallerHierarchy { unit } });
                    } else
                        throw "Unsupported";
                }
            };
        }

        template <auto f>
        constexpr SyncFunction syncFunction()
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            using T = typename traits::class_type;
            using Tuple = typename traits::decay_argument_types::as_tuple;

            return {
                [](const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *args) {
                    const Tuple &argTuple = *static_cast<const Tuple *>(args);
                    for (FormattedBufferedStream &out : outStreams) {
                        write(out, argTuple, "Args");
                        out.endMessageWrite();
                    }
                },
                [](FormattedBufferedStream &out, const void *result) {
                    write(out, *static_cast<const patch_void_t<R> *>(result), "Result");
                    out.endMessageWrite();
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &in, uint16_t index, FunctionType type, PendingRequest &request) {
                    switch (type) {
                    case CALL: {
                        Tuple args;
                        STREAM_PROPAGATE_ERROR(read(in, args, "Args"));
                        writeFunctionAction(unit, index, &args, {}, request.mRequester, request.mRequesterTransactionId);
                        patch_void_t<R> result = invoke_patch_void(LIFT(TupleUnpacker::invokeExpand), f, static_cast<T *>(unit), args);
                        request.mPromise.setValue(result);
                    } break;
                    case QUERY: {
                        patch_void_t<R> result;
                        STREAM_PROPAGATE_ERROR(read(in, result, "Result"));
                        if (request.mRequesterTransactionId) {
                            writeFunctionResult(unit, index, &result, request.mRequester, request.mRequesterTransactionId);
                        }
                        request.mPromise.setValue(result);
                    } break;
                    }
                    return StreamResult {};
                },
                [](SyncableUnitBase *unit, FormattedBufferedStream &in, uint16_t index, FunctionType type, MessageId id) {
                    Tuple args;
                    STREAM_PROPAGATE_ERROR(read(in, args, "Args"));
                    ParticipantId answerId = in.id();
                    if (static_cast<T *>(unit)->isMaster()) {
                        if (type == CALL)
                            writeFunctionAction(unit, index, &args, {}, answerId, id);
                        patch_void_t<R> result = invoke_patch_void(LIFT(TupleUnpacker::invokeExpand), f, static_cast<T *>(unit), args);
                        if (type == QUERY && id != 0)
                            writeFunctionResult(unit, index, &result, answerId, id);
                    } else {
                        writeFunctionRequest(
                            unit, index, type, &args, answerId, id);
                    }
                    return StreamResult {};
                }
            };
        }
    }

}
}

#define SERIALIZETABLE_BEGIN_IMPL(T, Base)                                                \
    namespace Serialize_##T                                                               \
    {                                                                                     \
        static constexpr const ::Engine::Serialize::SerializeTable &(*baseType)() = Base; \
    }                                                                                     \
    namespace Engine {                                                                    \
        template <>                                                                       \
        struct LineStruct<Serialize::__serialize_impl__::SerializerTag, __LINE__> {       \
            using Ty = T;                                                                 \
            static constexpr const bool base = true;                                      \
            constexpr const Serialize::Serializer *data() const;                          \
        };                                                                                \
        template <>                                                                       \
        struct LineStruct<Serialize::__serialize_impl__::FunctionTag, __LINE__> {         \
            using Ty = T;                                                                 \
            static constexpr const bool base = true;                                      \
            static constexpr const size_t count = 0;                                      \
            constexpr const Serialize::SyncFunction *data() const { return nullptr; }     \
            template <auto g>                                                             \
            static constexpr uint16_t getIndex();                                         \
        };                                                                                \
    }

#define SERIALIZETABLE_INHERIT_BEGIN(T, Base) SERIALIZETABLE_BEGIN_IMPL(T, &serializeTable<Base>)
#define SERIALIZETABLE_BEGIN(T) SERIALIZETABLE_BEGIN_IMPL(T, nullptr)

#define SERIALIZETABLE_ENTRY(Ser)                                                                                                                       \
    namespace Engine {                                                                                                                                  \
        template <>                                                                                                                                     \
        struct LineStruct<Serialize::__serialize_impl__::SerializerTag, __LINE__> : Serialize::__serialize_impl__::SerializerLineStruct<__LINE__ - 1> { \
            constexpr const Serialize::Serializer *data() const                                                                                         \
            {                                                                                                                                           \
                if constexpr (Serialize::__serialize_impl__::SerializerLineStruct<__LINE__ - 1>::base)                                                  \
                    return &mData;                                                                                                                      \
                else                                                                                                                                    \
                    return Serialize::__serialize_impl__::SerializerLineStruct<__LINE__ - 1>::data();                                                   \
            }                                                                                                                                           \
            static constexpr const bool base = false;                                                                                                   \
            Serialize::Serializer mData = Ser;                                                                                                          \
        };                                                                                                                                              \
    }

#define SYNCFUNCTION(f)                                                                                                                             \
    namespace Engine {                                                                                                                              \
        template <>                                                                                                                                 \
        struct LineStruct<Serialize::__serialize_impl__::FunctionTag, __LINE__> : Serialize::__serialize_impl__::FunctionLineStruct<__LINE__ - 1> { \
            constexpr const Serialize::SyncFunction *data() const                                                                                   \
            {                                                                                                                                       \
                if constexpr (Serialize::__serialize_impl__::FunctionLineStruct<__LINE__ - 1>::base)                                                \
                    return &mData;                                                                                                                  \
                else                                                                                                                                \
                    return Serialize::__serialize_impl__::FunctionLineStruct<__LINE__ - 1>::data();                                                 \
            }                                                                                                                                       \
            static constexpr const bool base = false;                                                                                               \
            static constexpr const size_t count = Serialize::__serialize_impl__::FunctionLineStruct<__LINE__ - 1>::count + 1;                       \
            Serialize::SyncFunction mData = Serialize::__serialize_impl__::syncFunction<&Ty::f>();                                                  \
            template <auto g>                                                                                                                       \
            static constexpr uint16_t getIndex()                                                                                                    \
            {                                                                                                                                       \
                if constexpr (f_same_as<&Ty::f, g>)                                                                                                 \
                    return count - 1;                                                                                                               \
                else                                                                                                                                \
                    return Serialize::__serialize_impl__::FunctionLineStruct<__LINE__ - 1>::getIndex<g>();                                          \
            }                                                                                                                                       \
        };                                                                                                                                          \
    }

#define SERIALIZETABLE_END(T)                                                                                         \
    SERIALIZETABLE_ENTRY({ nullptr })                                                                                 \
    namespace Serialize_##T                                                                                           \
    {                                                                                                                 \
        static constexpr ::Engine::Serialize::__serialize_impl__::SerializerLineStruct<__LINE__> fields = {};         \
        static constexpr ::Engine::Serialize::__serialize_impl__::FunctionLineStruct<__LINE__> functions = {};        \
    };                                                                                                                \
    namespace Engine {                                                                                                \
        namespace Serialize {                                                                                         \
            namespace __serialize_impl__ {                                                                            \
                template <>                                                                                           \
                struct SyncFunctionTable<T> : ::Engine::Serialize::__serialize_impl__::FunctionLineStruct<__LINE__> { \
                };                                                                                                    \
            }                                                                                                         \
        }                                                                                                             \
    }                                                                                                                 \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Serialize_##T::baseType, ::Serialize_##T::fields.data(), ::Serialize_##T::functions.data(), std::derived_from<T, ::Engine::Serialize::TopLevelUnitBase> }), T);

#define FIELD(...) \
    SERIALIZETABLE_ENTRY(SINGLE_ARG(::Engine::Serialize::__serialize_impl__::field<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__)))))

#define ENCAPSULATED_FIELD(Name, Getter, Setter) \
    SERIALIZETABLE_ENTRY(SINGLE_ARG(::Engine::Serialize::__serialize_impl__::encapsulated_field<Ty, &Ty::Getter, &Ty::Setter>(#Name)))

#define ENCAPSULATED_POINTER(Name, Getter, Setter) \
    SERIALIZETABLE_ENTRY(SINGLE_ARG(::Engine::Serialize::__serialize_impl__::encapsulated_pointer<Ty, &Ty::Name, &Ty::Getter, &Ty::Setter>(#Name)))
