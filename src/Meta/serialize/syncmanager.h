#pragma once

#include "serializemanager.h"

#include "Generic/genericresult.h"

#include "Generic/timeout.h"

#include "Generic/container/mutable_set.h"

#include "streams/comparestreamid.h"

#include "streams/formattedbufferedstream.h"

namespace Engine {
namespace Serialize {

    ENUM_BASE(SyncManagerResult, GenericResult,
        STREAM_ERROR,
        TIMEOUT)

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
        bool addTopLevelItem(TopLevelUnitBase *unit, bool sendStateFlag = true);
        void removeTopLevelItem(TopLevelUnitBase *unit);
        void moveTopLevelItem(TopLevelUnitBase *oldUnit, TopLevelUnitBase *newUnit);

        FormattedBufferedStream &getSlaveMessageTarget();

        //bool isMessageAvailable();
        StreamResult receiveMessages(int msgCount = -1, TimeOut timeout = {});
        void sendMessages();
        StreamResult sendAndReceiveMessages();

        StreamResult convertPtr(FormattedSerializeStream &in, UnitId unit, SyncableUnitBase *&out);

        std::vector<ParticipantId> getMasterParticipantIds();
        size_t clientCount() const;

		static ParticipantId getParticipantId(SyncManager *manager);

        StreamResult fetchStreamError();

    protected:
        StreamResult receiveMessages(FormattedBufferedStream &stream, int &msgCount, TimeOut timeout = {});
        bool sendAllMessages(FormattedBufferedStream &stream, TimeOut timeout = {});

        FormattedBufferedStream *getSlaveStream();
        FormattedBufferedStream &getMasterStream(ParticipantId id);

        void removeAllStreams();
        SyncManagerResult setSlaveStream(FormattedBufferedStream &&stream, bool receiveState = true, TimeOut timeout = {});
        void removeSlaveStream();
        SyncManagerResult addMasterStream(FormattedBufferedStream &&stream, bool sendState = true);
        SyncManagerResult moveMasterStream(ParticipantId streamId, SyncManager *target);


        const std::set<TopLevelUnitBase *> &getTopLevelUnits() const;

        void sendState(FormattedBufferedStream &stream, SyncableUnitBase *unit);

        SyncManagerResult recordStreamError(StreamResult error);

        std::unique_ptr<SyncStreamData> createStreamData();

        bool mSlaveStreamInvalid = false;

    private:

        mutable_set<FormattedBufferedStream, CompareStreamId> mMasterStreams;
        std::optional<FormattedBufferedStream> mSlaveStream;

        std::set<TopLevelUnitBase *> mTopLevelUnits; //TODO: Sort by MasterId    

        bool mReceivingMasterState = false;

        StreamResult mStreamError;
    };
}
}
