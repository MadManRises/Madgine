#pragma once

#include "Generic/callerhierarchy.h"
#include "Generic/execution/sender.h"
#include "serializableunit.h"

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

        StreamResult applyMap(FormattedSerializeStream &in, bool success, CallerHierarchyBasePtr hierarchy = {});
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
        friend META_EXPORT void writeFunctionRequest(SyncableUnitBase *unit, uint16_t index, FunctionType type, const void *args, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver);

        void writeFunctionAction(uint16_t index, const void *args, const std::set<ParticipantId> &targets = {}, ParticipantId answerTarget = 0, MessageId answerId = 0);
        void writeFunctionResult(uint16_t index, const void *result, ParticipantId answerTarget, MessageId answerId);
        void writeFunctionRequest(uint16_t index, FunctionType type, const void *args, ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessageReceiver receiver = {});

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
        auto call(Args &&...args)
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            return make_message_sender<R>(
                [this](auto &receiver, Args &&...args) {
                    typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                    if (this->isMaster()) {
                        this->writeFunctionAction(functionIndex<f>, &argTuple);
                        if constexpr (std::same_as<decltype(TupleUnpacker::invokeExpand(f, static_cast<T*>(this), argTuple)), void>) {
                            TupleUnpacker::invokeExpand(f, static_cast<T *>(this), argTuple);
                            receiver.set_value();
                        } else {
                            receiver.set_value(TupleUnpacker::invokeExpand(f, static_cast<T *>(this), argTuple));
                        }
                    } else {
                        this->writeFunctionRequest(functionIndex<f>, CALL, &argTuple, 0, 0, receiver);
                    }
                },
                std::forward<Args>(args)...);
        }

        template <auto f, typename... Args>
        void notify(const std::set<ParticipantId> &targets, Args &&...args)
        {
            assert(this->isMaster());
            if (!targets.empty()) {
                using traits = typename Callable<f>::traits;
                typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                this->writeFunctionAction(functionIndex<f>, &argTuple, targets);
            }
        }

        template <auto f, typename... Args>
        auto query(Args &&...args)
        {
            using traits = typename Callable<f>::traits;
            using R = typename traits::return_type;
            return make_message_sender<R>(
                [this](auto &receiver, Args &&...args) {
                    if (this->isMaster()) {
                        if constexpr (std::same_as<decltype((static_cast<T *>(this)->*f)(std::forward<Args>(args)...)), void>) {
                            (static_cast<T *>(this)->*f)(std::forward<Args>(args)...);
                            receiver.set_value();
                        } else {
                            receiver.set_value((static_cast<T *>(this)->*f)(std::forward<Args>(args)...));
                        }
                    } else {
                        typename traits::decay_argument_types::as_tuple argTuple { std::forward<Args>(args)... };
                        this->writeFunctionRequest(functionIndex<f>, QUERY, &argTuple, 0, 0, receiver);
                    }
                },
                std::forward<Args>(args)...);
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
                this->writeFunctionRequest(functionIndex<f>, QUERY, &argTuple);
            }
        }

        using _Base::_Base;
    };

} // namespace Serialize
} // namespace Core
