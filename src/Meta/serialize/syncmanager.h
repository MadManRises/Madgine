#pragma once

#include "serializableids.h"

#include "streams/bufferedstream.h"

#include "streams/comparestreamid.h"

#include "Generic/timeout.h"

#include "serializemanager.h"

#include "Generic/genericresult.h"

#include "Generic/container/nonconst_set.h"

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

        void readMessage(BufferedInOutStream &);

        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets();

        void clearTopLevelItems();
        bool addTopLevelItem(TopLevelUnitBase *unit, bool sendStateFlag = true);
        void removeTopLevelItem(TopLevelUnitBase *unit);
        void moveTopLevelItem(TopLevelUnitBase *oldUnit, TopLevelUnitBase *newUnit);

        BufferedOutStream &getSlaveMessageTarget();

        bool isMessageAvailable();
        void receiveMessages(int msgCount = -1, TimeOut timeout = {});
        void sendMessages();
        void sendAndReceiveMessages();

        SyncableUnitBase *convertPtr(SerializeInStream &in, UnitId unit);

        std::vector<ParticipantId> getMasterParticipantIds();
        size_t clientCount() const;

		static ParticipantId getParticipantId(SyncManager *manager);

        StreamState getStreamError() const;

    protected:
        bool receiveMessages(BufferedInOutStream &stream, int &msgCount, TimeOut timeout = {});
        bool sendAllMessages(BufferedInOutStream &stream, TimeOut timeout = {});

        BufferedInOutStream *getSlaveStream();
        std::set<BufferedInOutStream, CompareStreamId> &getMasterStreams();

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

        NonConstSet<BufferedInOutStream, CompareStreamId> mMasterStreams;
        std::optional<BufferedInOutStream> mSlaveStream;

        std::set<TopLevelUnitBase *> mTopLevelUnits; //TODO: Sort by MasterId    

        bool mSlaveStreamInvalid;

        StreamState mStreamError = StreamState::OK;
    };
}
}
