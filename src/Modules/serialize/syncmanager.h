#pragma once

#include "serializableids.h"

#include "streams/bufferedstream.h"

#include "streams/comparestreamid.h"

#include "Interfaces/util/timeout.h"

#include "serializemanager.h"

#include "Interfaces/util/genericresult.h"

namespace Engine {
namespace Serialize {

    ENUM_BASE(SyncManagerResult, GenericResult,
        STREAM_ERROR,
        TIMEOUT);

    struct MODULES_EXPORT SyncManager : SerializeManager {
        SyncManager(const std::string &name);
        SyncManager(const SerializeManager &) = delete;
        SyncManager(SyncManager &&) noexcept;
        ~SyncManager();

        void readMessage(BufferedInOutStream &);

        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets();

        void clearTopLevelItems();
        bool addTopLevelItem(TopLevelSerializableUnitBase *unit, bool sendStateFlag = true);
        void removeTopLevelItem(TopLevelSerializableUnitBase *unit);
        void moveTopLevelItem(TopLevelSerializableUnitBase *oldUnit, TopLevelSerializableUnitBase *newUnit);

        BufferedOutStream *getSlaveMessageTarget();

        bool isMessageAvailable();
        void receiveMessages(int msgCount = -1, TimeOut timeout = {});
        void sendMessages();
        void sendAndReceiveMessages();

        SerializableUnitBase *convertPtr(SerializeInStream &in, UnitId unit);

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


        const std::set<TopLevelSerializableUnitBase *> &getTopLevelUnits() const;

        void sendState(BufferedInOutStream &stream, SerializableUnitBase *unit);

        SyncManagerResult recordStreamError(StreamState error);

    private:

        bool mReceivingMasterState;

        std::set<BufferedInOutStream, CompareStreamId> mMasterStreams;
        std::optional<BufferedInOutStream> mSlaveStream;

        std::set<TopLevelSerializableUnitBase *> mTopLevelUnits;    //TODO: Sort by MasterId    

        bool mSlaveStreamInvalid;

        StreamState mStreamError = StreamState::OK;
    };
}
}
