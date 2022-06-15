#pragma once

#include "Generic/callerhierarchy.h"
#include "serializableunit.h"

#include "Generic/future.h"
#include "Generic/lambda.h"

#include "../streams/pendingrequest.h"

namespace Engine {
namespace Serialize {

#define SYNCABLEUNIT_MEMBERS()            \
    SERIALIZABLEUNIT_MEMBERS()            \
    READONLY_PROPERTY(MasterId, masterId) \
    READONLY_PROPERTY(SlaveId, slaveId)

    template <auto f>
    constexpr uint16_t functionIndex = __serialize_impl__::SyncFunctionTable<typename Callable<f>::traits::class_type>::template getIndex<f>();

    struct META_EXPORT SyncableUnitBase : SerializableUnitBase {
    protected:
        SyncableUnitBase(UnitId masterId = 0);
        SyncableUnitBase(const SyncableUnitBase &other);
        SyncableUnitBase(SyncableUnitBase &&other) noexcept;
        ~SyncableUnitBase();

        SyncableUnitBase &operator=(const SyncableUnitBase &other);
        SyncableUnitBase &operator=(SyncableUnitBase &&other);

    public:
        void writeState(FormattedSerializeStream &out, const char *name = nullptr, CallerHierarchyBasePtr hierarchy = {}, StateTransmissionFlags flags = 0) const;
        StreamResult readState(FormattedSerializeStream &in, const char *name = nullptr, CallerHierarchyBasePtr hierarchy = {}, StateTransmissionFlags flags = 0);

        StreamResult applyMap(FormattedSerializeStream &in, bool success);
        void setActive(bool active, bool existenceChanged);

        StreamResult readAction(FormattedBufferedStream &in, PendingRequest &request);
        StreamResult readRequest(FormattedBufferedStream &in, MessageId id);

        StreamResult readFunctionAction(FormattedBufferedStream &in, PendingRequest &request);
        StreamResult readFunctionRequest(FormattedBufferedStream &in, MessageId id);

        UnitId slaveId() const;
        UnitId masterId() const;

        bool isMaster() const;

    protected:
        void setSlaveId(UnitId id, SerializeManager *mgr);

        const SerializeTable *serializeType() const;

        UnitId moveMasterId(UnitId newId = 0);

        friend META_EXPORT void writeFunctionAction(SyncableUnitBase *unit, uint16_t index, const void *args, const std::set<ParticipantId> &targets, ParticipantId answerTarget, MessageId answerId);
        friend META_EXPORT void writeFunctionResult(SyncableUnitBase *unit, uint16_t index, const void *result, ParticipantId answerTarget, MessageId answerId);
        friend META_EXPORT void writeFunctionRequest(SyncableUnitBase *unit, uint16_t index, FunctionType type, const void *args, ParticipantId requester, MessageId requesterTransactionId, GenericMessagePromise promise);

        void writeFunctionAction(uint16_t index, const void *args, const std::set<ParticipantId> &targets = {}, ParticipantId answerTarget = 0, MessageId answerId = 0);
        void writeFunctionResult(uint16_t index, const void *result, ParticipantId answerTarget, MessageId answerId);
        void writeFunctionRequest(uint16_t index, FunctionType type, const void *args, ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessagePromise promise = {});
        template <typename T>
        MessageFuture<T> writeFunctionRequest(uint16_t index, FunctionType type, const void *args, ParticipantId requester = 0, MessageId requesterTransactionId = 0)
        {
            MessagePromise<T> p;
            MessageFuture<T> f { p.get_future() };
            writeFunctionRequest(index, type, args, requester, requesterTransactionId, std::move(p));
            return f;
        }

    private:
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterMessageTargets(const std::set<ParticipantId> &targets = {}) const;
        FormattedBufferedStream &getMasterRequestResponseTarget(ParticipantId answerTarget) const;
        FormattedBufferedStream &getSlaveMessageTarget() const;

        void clearSlaveId(SerializeManager *mgr);

        friend struct SyncManager;
        friend struct SerializeUnitHelper;
        friend struct SerializableUnitPtr;
        friend struct SerializableUnitConstPtr;
        friend struct SerializableDataPtr;
        friend struct SerializableDataConstPtr;
        friend struct SyncableBase;
        friend struct SerializeTable;
        template <typename T>
        friend struct UnitHelper;
        template <typename T, typename Base>
        friend struct TableInitializer;
        template <typename T>
        friend struct Syncable;
        template <typename T>
        friend struct Serializable;

        DERIVE_FRIEND(customUnitPtr)

        SerializableUnitPtr customUnitPtr();
        SerializableUnitConstPtr customUnitPtr() const;

    private:
        UnitId mSlaveId = 0;
        UnitId mMasterId;

        const SerializeTable *mType = nullptr;
    };

    template <typename T, typename Base>
    struct SyncableUnit;

    template <typename T, typename Base>
    struct TableInitializer {
        TableInitializer()
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(const TableInitializer &)
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(TableInitializer &&)
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer &operator=(const TableInitializer &)
        {
            return *this;
        }
        TableInitializer &operator=(TableInitializer &&)
        {
            return *this;
        }
    };

    template <typename T, typename _Base = SyncableUnitBase>
    struct SyncableUnit : _Base, private TableInitializer<T, _Base> {
    protected:
        friend TableInitializer<T, _Base>;

        template <auto f, typename... Args>
        MessageFuture<typename Callable<f>::traits::return_type> call(Args &&...args)
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
            if (this->isMaster()) {
                this->writeFunctionAction(functionIndex<f>, &argTuple);
                return invoke_patch_void(LIFT(TupleUnpacker::invokeExpand), f, static_cast<T *>(this), argTuple);
            } else {
                return this->template writeFunctionRequest<R>(
                    functionIndex<f>, CALL,
                    &argTuple);
            }
        }

        template <auto f, typename... Args>
        void notify(const std::set<ParticipantId> &targets, Args &&...args)
        {
            if (!targets.empty()) {
                using traits = typename Callable<f>::traits;
                typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                assert(this->isMaster());
                this->writeFunctionAction(functionIndex<f>, &argTuple, targets);
            }
        }

        template <auto f, typename... Args>
        MessageFuture<typename Callable<f>::traits::return_type> query(Args &&...args)
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            if (this->isMaster()) {
                return invoke_patch_void(f, static_cast<T *>(this), std::forward<Args>(args)...);
            } else {
                typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                return this->template writeFunctionRequest<R>(
                    functionIndex<f>, QUERY,
                    &argTuple);
            }
        }

        template <auto f, typename... Args>
        void command(Args &&...args)
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            if (this->isMaster()) {
                (static_cast<T *>(this)->*f)(std::forward<Args>(args)...);
            } else {
                typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                this->writeFunctionRequest(
                    functionIndex<f>, QUERY,
                    &argTuple);
            }
        }

        using _Base::_Base;
    };

} // namespace Serialize
} // namespace Core
