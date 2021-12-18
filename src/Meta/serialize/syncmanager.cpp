#include "../metalib.h"

#include "syncmanager.h"

#include "toplevelunit.h"

#include "messageheader.h"

#include "streams/buffered_streambuf.h"

#include "streams/operations.h"

#include "serializableids.h"

#include "serializetable.h"

namespace Engine {
namespace Serialize {

    SyncManager::SyncManager(const std::string &name)
        : SerializeManager(name)
        , mReceivingMasterState(false)
        , mSlaveStreamInvalid(false)
    {
    }

    SyncManager::SyncManager(SyncManager &&other) noexcept
        : SerializeManager(std::move(other))
        , mReceivingMasterState(other.mReceivingMasterState)
        , mTopLevelUnits(std::move(other.mTopLevelUnits))
        , mSlaveStreamInvalid(other.mSlaveStreamInvalid)
    {
        for (TopLevelUnitBase *unit : mTopLevelUnits) {
            unit->removeManager(&other);
            bool result = unit->addManager(this);
            assert(result);
        }
        other.mTopLevelUnits.clear();
        if (other.mSlaveStream) {
            mSlaveStream.emplace(std::move(*other.mSlaveStream), this);
            other.mSlaveStream.reset();
        }
        for (BufferedInOutStream &stream : other.mMasterStreams) {
            mMasterStreams.emplace(std::move(stream), this);
        }
    }

    SyncManager::~SyncManager() { clearTopLevelItems(); }

    StreamResult SyncManager::readMessage(BufferedInOutStream &stream)
    {
        MessageHeader header;
        STREAM_PROPAGATE_ERROR(stream.readHeader(header));

        if (header.mObject == SERIALIZE_MANAGER) {
            ParticipantId id;
            switch (header.mCmd) {
            case INITIAL_STATE_DONE:
                mReceivingMasterState = false;
                STREAM_PROPAGATE_ERROR(read(stream, id, "id"));
                stream.setId(id);
                break;
            default:
                return STREAM_INTEGRITY_ERROR(stream, "Invalid command used in message header: " << header.mCmd);
            }
        } else {
            SyncableUnitBase *object;
            STREAM_PROPAGATE_ERROR(convertPtr(stream, header.mObject, object));
            switch (header.mType) {
            case ACTION: {
                PendingRequest *request = stream.fetchRequest(header.mTransaction);
                STREAM_PROPAGATE_ERROR(object->readAction(stream, request));
                if (request)
                    stream.popRequest(header.mTransaction);
                break;
            }
            case REQUEST:
                STREAM_PROPAGATE_ERROR(object->readRequest(stream, header.mTransaction));
                break;
            case STATE:
                STREAM_PROPAGATE_ERROR(object->readState(stream, nullptr, {}, StateTransmissionFlags_ApplyMap));
                break;
            default:
                return STREAM_INTEGRITY_ERROR(stream, "Invalid Message-Type: " << header.mType);
            }
        }
        return {};
    }

    std::set<BufferedOutStream *, CompareStreamId>
    SyncManager::getMasterMessageTargets()
    {
        std::set<BufferedOutStream *, CompareStreamId> result;

        for (BufferedInOutStream &stream : mMasterStreams) {
            //if (!stream.isClosed()) {
            result.insert(&stream);
            //}
        }
        return result;
    }

    void SyncManager::clearTopLevelItems()
    {
        while (!slavesMap().empty()) {
            slavesMap().begin()->second->clearSlaveId(this);
        }
        for (TopLevelUnitBase *unit : mTopLevelUnits) {
            unit->removeManager(this);
        }
        mTopLevelUnits.clear();
    }

    bool SyncManager::addTopLevelItem(TopLevelUnitBase *unit,
        bool sendStateFlag)
    {
        if (!unit->addManager(this))
            return false;
        mTopLevelUnits.insert(unit);

        if (mSlaveStream) {
            unit->initSlaveId(this);
        }

        if (sendStateFlag && unit->mSynced) {
            for (BufferedInOutStream &stream : mMasterStreams) {
                this->sendState(stream, unit);
            }
        }
        return true;
    }

    void SyncManager::removeTopLevelItem(TopLevelUnitBase *unit)
    {
        auto it2 = slavesMap().begin();
        while (it2 != slavesMap().end()) {
            if (it2->second->mTopLevel == unit) {
                it2++->second->clearSlaveId(this);
            } else {
                ++it2;
            }
        }
        unit->removeManager(this);

        mTopLevelUnits.erase(unit);
    }

    void SyncManager::moveTopLevelItem(TopLevelUnitBase *oldUnit,
        TopLevelUnitBase *newUnit)
    {
        removeTopLevelItem(oldUnit);
        addTopLevelItem(newUnit, false);
    }

    BufferedOutStream &SyncManager::getSlaveMessageTarget()
    {
        assert(mSlaveStream);
        return *mSlaveStream;
    }

    bool SyncManager::isMessageAvailable()
    {
        if (mSlaveStream && mSlaveStream->isMessageAvailable())
            return true;
        for (BufferedInOutStream &stream : mMasterStreams) {
            if (stream.isMessageAvailable())
                return true;
        }
        return false;
    }

    void SyncManager::removeAllStreams()
    {
        removeSlaveStream();
        mMasterStreams.clear();
    }

    SyncManagerResult SyncManager::setSlaveStream(BufferedInOutStream &&stream,
        bool receiveState,
        TimeOut timeout)
    {
        if (mSlaveStream)
            return SyncManagerResult::UNKNOWN_ERROR;

        SyncManagerResult state = SyncManagerResult::SUCCESS;

        auto it = mTopLevelUnits.begin();

        for (; it != mTopLevelUnits.end(); ++it) {
            if (!(*it)->updateManagerType(this, false)) {
                state = SyncManagerResult::UNKNOWN_ERROR;
                break;
            }
        }

        mSlaveStream.emplace(std::move(stream));
        setSlaveStreamData(&mSlaveStream->data());

        if (receiveState) {
            if (state == SyncManagerResult::SUCCESS) {
                for (TopLevelUnitBase *unit : mTopLevelUnits) {
                    unit->initSlaveId(this);
                }
                mReceivingMasterState = true;
                while (mReceivingMasterState) {
                    int msgCount = -1;
                    if (StreamResult result = receiveMessages(*mSlaveStream, msgCount); result.mState != StreamState::OK) {
                        state = recordStreamError(result.mState);
                        mReceivingMasterState = false;
                    }
                    if (mReceivingMasterState && timeout.expired()) {
                        state = SyncManagerResult::TIMEOUT;
                        mReceivingMasterState = false;
                    }
                }
                if (state != SyncManagerResult::SUCCESS) {
                    while (!slavesMap().empty()) {
                        slavesMap().begin()->second->clearSlaveId(this);
                    }
                }
            }
        }

        if (state != SyncManagerResult::SUCCESS) {
            for (auto it2 = mTopLevelUnits.begin(); it2 != it; ++it2) {
                bool result = (*it2)->updateManagerType(this, true);
                assert(result);
            }
            setSlaveStreamData(nullptr);
            mSlaveStream.reset();
        }

        return state;
    }

    void SyncManager::removeSlaveStream()
    {
        if (mSlaveStream) {
            while (!slavesMap().empty()) {
                size_t s = slavesMap().size();
                slavesMap().begin()->second->clearSlaveId(this);
                assert(s > slavesMap().size());
            }
            for (TopLevelUnitBase *topLevel : mTopLevelUnits) {
                topLevel->updateManagerType(this, true);
            }
            mSlaveStream->setId(0);
            mSlaveStream.reset();
        }
    }

    SyncManagerResult SyncManager::addMasterStream(BufferedInOutStream &&stream,
        bool sendStateFlag)
    {
        if (sendStateFlag && stream) {
            for (TopLevelUnitBase *unit : mTopLevelUnits) {
                sendState(stream, unit);
            }
            stream.writeCommand(INITIAL_STATE_DONE, stream.id());
        }

        if (!stream)
            return recordStreamError(StreamState::UNKNOWN_ERROR);

        mMasterStreams.emplace(std::move(stream));
        return SyncManagerResult::SUCCESS;
    }

    SyncManagerResult SyncManager::moveMasterStream(ParticipantId streamId,
        SyncManager *target)
    {
        auto it = mMasterStreams.find(streamId);
        assert(it != mMasterStreams.end());
        if (SyncManagerResult result = target->addMasterStream(
                std::move(*it), false);
            result != SyncManagerResult::SUCCESS)
            return result;
        BufferedInOutStream &stream = *target->mMasterStreams.find(streamId);
        std::vector<TopLevelUnitBase *> newTopLevels;
        newTopLevels.reserve(16);
        set_difference(target->getTopLevelUnits().begin(),
            target->getTopLevelUnits().end(), getTopLevelUnits().begin(),
            getTopLevelUnits().end(), back_inserter(newTopLevels));
        for (TopLevelUnitBase *newTopLevel : newTopLevels) {
            sendState(stream, newTopLevel);
        }
        return SyncManagerResult::SUCCESS;
    }

    ParticipantId SyncManager::getParticipantId(SyncManager *manager)
    {
        if (manager && manager->mSlaveStream) {
            return manager->mSlaveStream->id();
        } else {
            return sLocalMasterParticipantId;
        }
    }

    StreamState SyncManager::getStreamError() const
    {
        return mStreamError;
    }

    StreamResult SyncManager::receiveMessages(int msgCount, TimeOut timeout)
    {
        if (mSlaveStream && !mSlaveStreamInvalid) {
            if (StreamResult result = receiveMessages(*mSlaveStream, msgCount, timeout); result.mState != StreamState::OK) {
                removeSlaveStream();
                return result;
            }
        }
        for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();) {
            if (StreamResult result = receiveMessages(*it, msgCount,
                    timeout);
                result.mState != StreamState::OK) {
                it = mMasterStreams.erase(it);
                return result;
            } else {
                ++it;
            }
        }
        return {};
    }

    void SyncManager::sendMessages()
    {
        if (mSlaveStream && !mSlaveStreamInvalid) {
            if (!mSlaveStream->sendMessages()) {
                removeSlaveStream();
            }
        }
        for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();) {
            if (!it->sendMessages()) {
                it = mMasterStreams.erase(it);
            } else {
                ++it;
            }
        }
    }

    StreamResult SyncManager::sendAndReceiveMessages()
    {
        STREAM_PROPAGATE_ERROR(receiveMessages());
        sendMessages();
        return {};
    }

    StreamResult SyncManager::convertPtr(SerializeInStream &in,
        UnitId unit, SyncableUnitBase *&out)
    {
        if (unit == NULL_UNIT_ID) {
            out = nullptr;
            return {};
        }
        try {
            if (mSlaveStream && (&in == &*mSlaveStream)) {
                out = slavesMap().at(unit);
            } else {
                if (unit < RESERVED_ID_COUNT) {
                    assert(unit >= BEGIN_STATIC_ID_SPACE);
                    auto it = std::find_if(
                        mTopLevelUnits.begin(), mTopLevelUnits.end(),
                        [unit](TopLevelUnitBase *topLevel) {
                            return topLevel->masterId() == unit;
                        });
                    if (it == mTopLevelUnits.end()) {
                        return STREAM_INTEGRITY_ERROR(in, "Illegal TopLevel-Id (" << unit << ") used!");
                    }
                    out = *it;
                } else {
                    SyncableUnitBase *u = getByMasterId(unit);
                    if (std::find(mTopLevelUnits.begin(), mTopLevelUnits.end(), u->mTopLevel) == mTopLevelUnits.end()) {
                        return STREAM_INTEGRITY_ERROR(in, "Unit (" << unit << ") with unregistered TopLevel-Unit used!");
                    }
                    out = u;
                }                
            }
        } catch (const std::out_of_range &) {
            return STREAM_INTEGRITY_ERROR(in, "Unknown Syncable Unit-Id (" << unit << ") used!");
        }
        return {};
    }

    std::vector<ParticipantId> SyncManager::getMasterParticipantIds()
    {
        std::vector<ParticipantId> result;
        result.reserve(mMasterStreams.size());
        for (const BufferedInOutStream &stream : mMasterStreams) {
            result.push_back(stream.id());
        }
        return result;
    }

    StreamResult SyncManager::receiveMessages(BufferedInOutStream &stream,
        int &msgCount, TimeOut timeout)
    {

        while (stream && ((stream.isMessageAvailable() && msgCount == -1) || msgCount > 0)) {
            while (stream.isMessageAvailable() && msgCount != 0) {
                STREAM_PROPAGATE_ERROR(readMessage(stream));
                if (msgCount > 0)
                    --msgCount;
                if (!timeout.isZero() && timeout.expired())
                    break;
            }
            if (timeout.expired())
                break;
        }

        return {};
    }

    bool SyncManager::sendAllMessages(BufferedInOutStream &stream,
        TimeOut timeout)
    {
        //TODO: Use Timeout (possibly pass down)
        return bool(stream.sendMessages());
    }

    BufferedInOutStream *SyncManager::getSlaveStream()
    {
        return mSlaveStream ? &*mSlaveStream : nullptr;
    }

    BufferedInOutStream &SyncManager::getMasterStream(ParticipantId id)
    {
        return *mMasterStreams.find(id);
    }

    const std::set<TopLevelUnitBase *> &
    SyncManager::getTopLevelUnits() const
    {
        return mTopLevelUnits;
    }

    size_t SyncManager::clientCount() const
    {
        return mMasterStreams.size();
    }

    void SyncManager::sendState(BufferedInOutStream &stream,
        SyncableUnitBase *unit)
    {
        stream.beginMessage(unit, STATE, 0);
        unit->writeState(stream);
        stream.endMessage();
    }

    SyncManagerResult SyncManager::recordStreamError(StreamState error)
    {
        mStreamError = error;
        return SyncManagerResult::STREAM_ERROR;
    }

} // namespace Serialize
} // namespace Engine
