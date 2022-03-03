#include "../metalib.h"

#include "syncmanager.h"

#include "hierarchy/toplevelunit.h"

#include "hierarchy/serializableids.h"

#include "hierarchy/statetransmissionflags.h"

#include "streams/syncstreamdata.h"

#include "operations.h"

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
        for (FormattedBufferedStream &stream : other.mMasterStreams) {
            mMasterStreams.emplace(std::move(stream), this);
        }
    }

    SyncManager::~SyncManager() { clearTopLevelItems(); }

    void SyncManager::writeHeader(FormattedBufferedStream &stream, const SyncableUnitBase *unit, MessageType type, TransactionId id)
    {
        stream.beginHeaderWrite();
        write(stream, SerializeManager::convertPtr(stream.stream(), unit), "Object");
        write(stream, type, "MessageType");
        write(stream, id, "TransactionId");
        stream.endHeaderWrite();
    }

    StreamResult SyncManager::readMessage(FormattedBufferedStream &stream)
    {
        STREAM_PROPAGATE_ERROR(stream.beginHeaderRead());
        UnitId objectId;
        STREAM_PROPAGATE_ERROR(read(stream, objectId, "Object"));

        if (objectId == SERIALIZE_MANAGER) {
            ParticipantId id;
            Command cmd;
            STREAM_PROPAGATE_ERROR(read(stream, cmd, "Command"));
            STREAM_PROPAGATE_ERROR(stream.endHeaderRead());
            switch (cmd) {
            case INITIAL_STATE_DONE:
                mReceivingMasterState = false;
                STREAM_PROPAGATE_ERROR(read(stream, id, "Id"));
                stream.setId(id);
                break;
            default:
                return STREAM_INTEGRITY_ERROR(stream.stream(), stream.isBinary(), "Invalid command used in message header: " << cmd);
            }
        } else {
            SyncableUnitBase *object;
            STREAM_PROPAGATE_ERROR(convertPtr(stream, objectId, object));
            MessageType type;
            STREAM_PROPAGATE_ERROR(read(stream, type, "MessageType"));
            TransactionId transactionId;
            STREAM_PROPAGATE_ERROR(read(stream, transactionId, "TransactionId"));
            STREAM_PROPAGATE_ERROR(stream.endHeaderRead());
            switch (type) {
            case ACTION: {
                PendingRequest *request = stream.fetchRequest(transactionId);
                STREAM_PROPAGATE_ERROR(object->readAction(stream, request));
                if (request)
                    stream.popRequest(transactionId);
                break;
            }
            case REQUEST:
                STREAM_PROPAGATE_ERROR(object->readRequest(stream, transactionId));
                break;
            case STATE:
                STREAM_PROPAGATE_ERROR(object->readState(stream, nullptr, {}, StateTransmissionFlags_ApplyMap));
                break;
            default:
                return STREAM_INTEGRITY_ERROR(stream.stream(), stream.isBinary(), "Invalid Message-Type: " << type);
            }
        }
        return {};
    }

    std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId>
    SyncManager::getMasterMessageTargets()
    {
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> result;

        for (FormattedBufferedStream &stream : mMasterStreams) {
            //if (!stream.isClosed()) {
            result.insert(stream);
            //}
        }
        return result;
    }

    void SyncManager::clearTopLevelItems()
    {
        while (!mSlaveMappings.empty()) {
            mSlaveMappings.begin()->second->clearSlaveId(this);
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
            for (FormattedBufferedStream &stream : mMasterStreams) {
                this->sendState(stream, unit);
            }
        }
        return true;
    }

    void SyncManager::removeTopLevelItem(TopLevelUnitBase *unit)
    {
        auto it2 = mSlaveMappings.begin();
        while (it2 != mSlaveMappings.end()) {
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

    FormattedBufferedStream &SyncManager::getSlaveMessageTarget()
    {
        assert(mSlaveStream);
        return *mSlaveStream;
    }

    /* bool SyncManager::isMessageAvailable()
    {
        if (mSlaveStream && mSlaveStream->isMessageAvailable())
            return true;
        for (FormattedBufferedStream &stream : mMasterStreams) {
            if (stream.isMessageAvailable())
                return true;
        }
        return false;
    }*/

    void SyncManager::removeAllStreams()
    {
        removeSlaveStream();
        mMasterStreams.clear();
    }

    SyncManagerResult SyncManager::setSlaveStream(FormattedBufferedStream &&stream,
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

        mSlaveStreamInvalid = true;
        mSlaveStream.emplace(std::move(stream));
        setSlaveStreamData(mSlaveStream->data());

        if (receiveState) {
            if (state == SyncManagerResult::SUCCESS) {
                for (TopLevelUnitBase *unit : mTopLevelUnits) {
                    unit->initSlaveId(this);
                }
                mReceivingMasterState = true;
                while (mReceivingMasterState) {
                    int msgCount = -1;
                    if (StreamResult result = receiveMessages(*mSlaveStream, msgCount); result.mState != StreamState::OK) {
                        state = recordStreamError(std::move(result));
                        mReceivingMasterState = false;
                    }
                    if (mReceivingMasterState && timeout.expired()) {
                        state = SyncManagerResult::TIMEOUT;
                        mReceivingMasterState = false;
                    }
                }
                if (state != SyncManagerResult::SUCCESS) {
                    while (!mSlaveMappings.empty()) {
                        mSlaveMappings.begin()->second->clearSlaveId(this);
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
            mReceivingMasterState = false;
        } else {
            mSlaveStreamInvalid = false;
        }

        return state;
    }

    void SyncManager::removeSlaveStream()
    {
        if (mSlaveStream) {
            while (!mSlaveMappings.empty()) {
                size_t s = mSlaveMappings.size();
                mSlaveMappings.begin()->second->clearSlaveId(this);
                assert(s > mSlaveMappings.size());
            }
            for (TopLevelUnitBase *topLevel : mTopLevelUnits) {
                topLevel->updateManagerType(this, true);
            }
            mSlaveStream->setId(0);
            mSlaveStream.reset();
        }
    }

    SyncManagerResult SyncManager::addMasterStream(FormattedBufferedStream &&stream,
        bool sendStateFlag)
    {
        if (sendStateFlag && stream) {
            for (TopLevelUnitBase *unit : mTopLevelUnits) {
                sendState(stream, unit);
            }
            stream.beginMessageWrite();
            stream.beginHeaderWrite();
            write<UnitId>(stream, SERIALIZE_MANAGER, "Object");
            write(stream, INITIAL_STATE_DONE, "Command");
            stream.endHeaderWrite();
            write(stream, stream.id(), "Id");
            stream.endMessageWrite();
        }

        if (!stream)
            return SyncManagerResult::UNKNOWN_ERROR;

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
        FormattedBufferedStream &stream = *target->mMasterStreams.find(streamId);
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

    StreamResult SyncManager::fetchStreamError()
    {
        return std::move(mStreamError);
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
            } else if (!*it) {
                it = mMasterStreams.erase(it);
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

    StreamResult SyncManager::convertPtr(FormattedSerializeStream &in,
        UnitId unit, SyncableUnitBase *&out)
    {
        if (unit == NULL_UNIT_ID) {
            out = nullptr;
            return {};
        }
        try {
            if (mSlaveStream && (&in == &*mSlaveStream)) {
                out = mSlaveMappings.at(unit);
            } else {
                if (unit < RESERVED_ID_COUNT) {
                    assert(unit >= BEGIN_STATIC_ID_SPACE);
                    auto it = std::ranges::find(
                        mTopLevelUnits, unit, &TopLevelUnitBase::masterId);
                    if (it == mTopLevelUnits.end()) {
                        return STREAM_INTEGRITY_ERROR(in.stream(), in.isBinary(), "Illegal TopLevel-Id (" << unit << ") used!");
                    }
                    out = *it;
                } else {
                    SyncableUnitBase *u = getByMasterId(unit);
                    if (!u)
                        return STREAM_INTEGRITY_ERROR(in.stream(), in.isBinary(), "Non-existing Unit-Id (" << unit << ") used!");
                    if (std::ranges::find(mTopLevelUnits, u->mTopLevel) == mTopLevelUnits.end())
                        return STREAM_INTEGRITY_ERROR(in.stream(), in.isBinary(), "Unit (" << unit << ") with unregistered TopLevel-Unit used!");

                    out = u;
                }
            }
        } catch (const std::out_of_range &) {
            return STREAM_INTEGRITY_ERROR(in.stream(), true, "Unknown Syncable Unit-Id (" << unit << ") used!");
        }
        return {};
    }

    std::vector<ParticipantId> SyncManager::getMasterParticipantIds()
    {
        std::vector<ParticipantId> result;
        result.reserve(mMasterStreams.size());
        for (const FormattedBufferedStream &stream : mMasterStreams) {
            result.push_back(stream.id());
        }
        return result;
    }

    StreamResult SyncManager::receiveMessages(FormattedBufferedStream &stream,
        int &msgCount, TimeOut timeout)
    {
        if (msgCount >= 0) {
            while (stream && msgCount > 0) {
                while (FormattedBufferedStream::MessageReadMarker msg = stream.beginMessageRead()) {
                    STREAM_PROPAGATE_ERROR(readMessage(stream));
                    msg.end();
                    --msgCount;
                    if (!timeout.isZero() && timeout.expired())
                        break;
                }
                if (timeout.expired())
                    break;
            }
        } else {
            while (FormattedBufferedStream::MessageReadMarker msg = stream.beginMessageRead()) {
                STREAM_PROPAGATE_ERROR(readMessage(stream));
                msg.end();
                if (!timeout.isZero() && timeout.expired())
                    break;
            }
        }

        return {};
    }

    bool SyncManager::sendAllMessages(FormattedBufferedStream &stream,
        TimeOut timeout)
    {
        //TODO: Use Timeout (possibly pass down)
        return bool(stream.sendMessages());
    }

    FormattedBufferedStream *SyncManager::getSlaveStream()
    {
        return mSlaveStream ? &*mSlaveStream : nullptr;
    }

    FormattedBufferedStream &SyncManager::getMasterStream(ParticipantId id)
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

    void SyncManager::sendState(FormattedBufferedStream &stream,
        SyncableUnitBase *unit)
    {
        stream.beginMessageWrite();
        stream.beginHeaderWrite();
        write(stream, SerializeManager::convertPtr(stream.stream(), unit), "Object");
        write(stream, STATE, "MessageType");
        write<TransactionId>(stream, 0, "TransactionId");
        stream.endHeaderWrite();
        unit->writeState(stream);
        stream.endMessageWrite();
    }

    SyncManagerResult SyncManager::recordStreamError(StreamResult error)
    {
        mStreamError = std::move(error);
        return SyncManagerResult::STREAM_ERROR;
    }

    std::unique_ptr<SyncStreamData> SyncManager::createStreamData()
    {
        return std::make_unique<SyncStreamData>(*this, createStreamId());
    }

} // namespace Serialize
} // namespace Engine
