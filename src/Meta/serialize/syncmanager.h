#pragma once

#include "streams/bufferedstream.h"

#include "streams/comparestreamid.h"

#include "Generic/timeout.h"

#include "serializemanager.h"

#include "Generic/genericresult.h"

#include "Generic/container/mutable_set.h"

namespace Engine {
namespace Serialize {

    ENUM_BASE(SyncManagerResult, GenericResult,
        STREAM_ERROR,
        TIMEOUT);

    struct META_EXPORT SyncManager : SerializeManager {
        SyncManager(const std::string &name);
        SyncManager(const SerializeManager &) = delete;
        SyncManager(SyncManager &&) noexcept;
        ~SyncManager();

        StreamResult readMessage(BufferedInOutStream &);

        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets();

        void clearTopLevelItems();
        bool addTopLevelItem(TopLevelUnitBase *unit, bool sendStateFlag = true);
        void removeTopLevelItem(TopLevelUnitBase *unit);
        void moveTopLevelItem(TopLevelUnitBase *oldUnit, TopLevelUnitBase *newUnit);

        BufferedOutStream &getSlaveMessageTarget();

        bool isMessageAvailable();
        StreamResult receiveMessages(int msgCount = -1, TimeOut timeout = {});
        void sendMessages();
        StreamResult sendAndReceiveMessages();

        StreamResult convertPtr(SerializeInStream &in, UnitId unit, SyncableUnitBase *&out);

        std::vector<ParticipantId> getMasterParticipantIds();
        size_t clientCount() const;

		static ParticipantId getParticipantId(SyncManager *manager);

        StreamState getStreamError() const;

    protected:
        StreamResult receiveMessages(BufferedInOutStream &stream, int &msgCount, TimeOut timeout = {});
        bool sendAllMessages(BufferedInOutStream &stream, TimeOut timeout = {});

        BufferedInOutStream *getSlaveStream();
        BufferedInOutStream &getMasterStream(ParticipantId id);

        void removeAllStreams();
        SyncManagerResult setSlaveStream(BufferedInOutStream &&stream, bool receiveState = true, TimeOut timeout = {});
        void removeSlaveStream();
        SyncManagerResult addMasterStream(BufferedInOutStream &&stream, bool sendState = true);
        SyncManagerResult moveMasterStream(ParticipantId streamId, SyncManager *target);


        const std::set<TopLevelUnitBase *> &getTopLevelUnits() const;

        void sendState(BufferedInOutStream &stream, SyncableUnitBase *unit);

        SyncManagerResult recordStreamError(StreamState error);

    private:

        bool mReceivingMasterState;

        mutable_set<BufferedInOutStream, CompareStreamId> mMasterStreams;
        std::optional<BufferedInOutStream> mSlaveStream;

        std::set<TopLevelUnitBase *> mTopLevelUnits; //TODO: Sort by MasterId    

        bool mSlaveStreamInvalid;

        StreamState mStreamError = StreamState::OK;
    };
}
}
