#pragma once

#include "serializemanager.h"

#include "Generic/genericresult.h"

#include "Generic/timeout.h"

#include "Generic/container/mutable_set.h"

#include "streams/comparestreamid.h"

#include "streams/formattedbufferedstream.h"

#include "Generic/execution/virtualsender.h"

#include "Generic/functor.h"

namespace Engine {
namespace Serialize {

    ENUM_BASE(SyncManagerResult, GenericResult,
        STREAM_ERROR,
        TIMEOUT,
        CANCELED)

    struct META_EXPORT SyncManager : SerializeManager {
        SyncManager(const std::string &name);
        SyncManager(const SerializeManager &) = delete;
        SyncManager(SyncManager &&) noexcept;
        ~SyncManager();

        static void writeHeader(FormattedBufferedStream &stream, const SyncableUnitBase *unit, MessageType type);
        static void writeActionHeader(FormattedBufferedStream &stream, const SyncableUnitBase *unit, MessageType type, MessageId id);
        StreamResult readMessage(FormattedBufferedStream &stream, MessageId id);

        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterMessageTargets();

        void clearTopLevelItems();
        void addTopLevelItemImpl(Execution::VirtualReceiverBase<SyncManagerResult> &receiver, TopLevelUnitBase *unit, std::string_view name);
        void addTopLevelItemImpl(Execution::VirtualReceiverBase<SyncManagerResult> &receiver, TopLevelUnitBase *unit, UnitId slaveId = 0);
        ASYNC_STUB(addTopLevelItem, addTopLevelItemImpl, Execution::make_simple_virtual_sender<SyncManagerResult>);
        void removeTopLevelItem(TopLevelUnitBase *unit);
        void moveTopLevelItem(TopLevelUnitBase *oldUnit, TopLevelUnitBase *newUnit);

        FormattedBufferedStream &getSlaveMessageTarget();

        StreamResult receiveMessages(int msgCount = -1, TimeOut timeout = {});
        void sendMessages();
        StreamResult sendAndReceiveMessages();

        StreamResult convertPtr(FormattedSerializeStream &in, UnitId unit, SyncableUnitBase *&out);

        std::vector<ParticipantId> getMasterParticipantIds();
        size_t clientCount() const;

        static ParticipantId getParticipantId(SyncManager *manager);

        StreamResult fetchStreamError();

        void setError(SyncableUnitBase *unit, PendingRequest &pending, MessageResult error);

    protected:
        StreamResult receiveMessages(FormattedBufferedStream &stream, int &msgCount, TimeOut timeout = {});
        bool sendAllMessages(FormattedBufferedStream &stream, TimeOut timeout = {});

        FormattedBufferedStream *getSlaveStream();
        FormattedBufferedStream &getMasterStream(ParticipantId id);

        void removeAllStreams();
        void setSlaveStreamImpl(Execution::VirtualReceiverBase<SyncManagerResult> &receiver, FormattedBufferedStream &&stream, TimeOut timeout = {});
        ASYNC_STUB(setSlaveStream, setSlaveStreamImpl, Execution::make_simple_virtual_sender<SyncManagerResult>);
        void decreaseReceivingCounter();

        void removeSlaveStream(SyncManagerResult reason = SyncManagerResult::UNKNOWN_ERROR);
        SyncManagerResult addMasterStream(FormattedBufferedStream &&stream);
        SyncManagerResult moveMasterStream(ParticipantId streamId, SyncManager *target);

        const std::set<TopLevelUnitBase *> &getTopLevelUnits() const;

        void sendState(FormattedBufferedStream &stream, SyncableUnitBase *unit);

        SyncManagerResult recordStreamError(StreamResult error);

        std::unique_ptr<SyncStreamData> createStreamData();

        Execution::VirtualReceiverBase<SyncManagerResult> *mReceivingMasterState = nullptr;
        TimeOut mReceivingMasterStateTimeout;
        size_t mReceivingCounter;

    private:
        std::map<ParticipantId, FormattedBufferedStream> mMasterStreams;
        std::optional<FormattedBufferedStream> mSlaveStream;

        std::set<TopLevelUnitBase *> mTopLevelUnits; //TODO: Sort by MasterId

        std::map<std::string, TopLevelUnitBase *> mTopLevelUnitNameMappings;

        StreamResult mStreamError;
    };
}
}
