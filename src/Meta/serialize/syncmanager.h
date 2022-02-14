#pragma once

#include "streams/formattedbufferedstream.h"
#include "streams/streamresult.h"

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

        StreamResult readMessage(FormattedBufferedStream &);

        std::set<FormattedBufferedStream *, CompareStreamId> getMasterMessageTargets();

        void clearTopLevelItems();
        bool addTopLevelItem(TopLevelUnitBase *unit, bool sendStateFlag = true);
        void removeTopLevelItem(TopLevelUnitBase *unit);
        void moveTopLevelItem(TopLevelUnitBase *oldUnit, TopLevelUnitBase *newUnit);

        FormattedBufferedStream &getSlaveMessageTarget();

        bool isMessageAvailable();
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

    private:

        bool mReceivingMasterState;

        mutable_set<FormattedBufferedStream, CompareStreamId> mMasterStreams;
        std::optional<FormattedBufferedStream> mSlaveStream;

        std::set<TopLevelUnitBase *> mTopLevelUnits; //TODO: Sort by MasterId    

        bool mSlaveStreamInvalid;

        StreamResult mStreamError;
    };
}
}
