#include "../moduleslib.h"

#include "syncmanager.h"

#include "toplevelserializableunit.h"

#include "messageheader.h"

#include "streams/buffered_streambuf.h"

#include "serializeexception.h"

#include "streams/operations.h"

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
        for (TopLevelSerializableUnitBase *unit : mTopLevelUnits) {
            unit->removeManager(&other);
            bool result = unit->addManager(this);
            assert(result);
        }
        other.mTopLevelUnits.clear();
        if (other.mSlaveStream) {
            mSlaveStream.emplace(std::move(*other.mSlaveStream), this);
            other.mSlaveStream.reset();
        }
        for (const BufferedInOutStream &stream : other.mMasterStreams) {
            mMasterStreams.emplace(std::move(const_cast<BufferedInOutStream &>(stream)), this);
        }
    }

    SyncManager::~SyncManager() { clearTopLevelItems(); }

    void SyncManager::readMessage(BufferedInOutStream &stream)
    {
        MessageHeader header;
        stream.readHeader(header);

        if (header.mObject == SERIALIZE_MANAGER) {
            stream.logReadHeader(header, name());
            ParticipantId id;
            switch (header.mCmd) {
            case INITIAL_STATE_DONE:
                mReceivingMasterState = false;
                stream >> id;
                stream.setId(id);
                break;
            case STREAM_EOF:
                stream.close();
                break;
            default:
                throw SerializeException(
                    "Unknown Builtin Command-Code for SerializeManager: " + std::to_string(header.mCmd));
            }
        } else {
            SerializableUnitBase *object = convertPtr(stream, header.mObject);
            stream.logReadHeader(header, typeid(*object).name());
            switch (header.mType) {
            case ACTION: {
                PendingRequest *request = stream.fetchRequest(header.mTransaction);
                object->readAction(stream, request);
                if (request)
                    stream.popRequest(header.mTransaction);
                break;
            }
            case REQUEST:
                object->readRequest(stream, header.mTransaction);
                break;
            case STATE:
                object->readState(stream);
                break;
            default:
                throw SerializeException("Invalid Message-Type: " + std::to_string(header.mType));
            }
        }
    }

    std::set<BufferedOutStream *, CompareStreamId>
    SyncManager::getMasterMessageTargets()
    {
        std::set<BufferedOutStream *, CompareStreamId> result;

        for (const BufferedInOutStream &stream : mMasterStreams) {
            if (!stream.isClosed()) {
                result.insert(const_cast<BufferedInOutStream *>(&stream));
            }
        }
        return result;
    }

    void SyncManager::clearTopLevelItems()
    {
        while (!slavesMap().empty()) {
            slavesMap().begin()->second->clearSlaveId(this);
        }
        for (TopLevelSerializableUnitBase *unit : mTopLevelUnits) {
            unit->removeManager(this);
        }
        mTopLevelUnits.clear();
    }

    bool SyncManager::addTopLevelItem(TopLevelSerializableUnitBase *unit,
        bool sendStateFlag)
    {
        if (!unit->addManager(this))
            return false;
        mTopLevelUnits.insert(unit);

        if (mSlaveStream) {
            unit->initSlaveId(this);
        }

        if (sendStateFlag && unit->isSynced()) {
            for (const BufferedInOutStream &stream : mMasterStreams) {
                this->sendState(const_cast<BufferedInOutStream &>(stream), unit);
            }
        }
        return true;
    }

    void SyncManager::removeTopLevelItem(TopLevelSerializableUnitBase *unit)
    {
        auto it2 = slavesMap().begin();
        while (it2 != slavesMap().end()) {
            if (it2->second->topLevel() == unit) {
                it2++->second->clearSlaveId(this);
            } else {
                ++it2;
            }
        }
        unit->removeManager(this);

        mTopLevelUnits.erase(unit);
    }

    void SyncManager::moveTopLevelItem(TopLevelSerializableUnitBase *oldUnit,
        TopLevelSerializableUnitBase *newUnit)
    {
        removeTopLevelItem(oldUnit);
        addTopLevelItem(newUnit, false);
    }

    BufferedOutStream *SyncManager::getSlaveMessageTarget()
    {
        return &*mSlaveStream;
    }

    bool SyncManager::isMessageAvailable()
    {
        if (mSlaveStream && mSlaveStream->isMessageAvailable())
            return true;
        for (const BufferedInOutStream &stream : mMasterStreams) {
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
        setSlaveStreambuf(&mSlaveStream->buffer());

        if (receiveState) {
            if (state == SyncManagerResult::SUCCESS) {
                for (TopLevelSerializableUnitBase *unit : mTopLevelUnits) {
                    unit->initSlaveId(this);
                }
                mReceivingMasterState = true;
                while (mReceivingMasterState) {
                    int msgCount = -1;
                    if (!receiveMessages(*mSlaveStream, msgCount)) {
                        state = recordStreamError(mSlaveStream->state());
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
            setSlaveStreambuf(nullptr);
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
            for (TopLevelSerializableUnitBase *topLevel : mTopLevelUnits) {
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
            for (TopLevelSerializableUnitBase *unit : mTopLevelUnits) {
                sendState(stream, unit);
            }
            stream.writeCommand(INITIAL_STATE_DONE, stream.id());
        }

        if (!stream)
            return recordStreamError(stream.state());

        mMasterStreams.emplace(std::move(stream));
        return SyncManagerResult::SUCCESS;
    }

    SyncManagerResult SyncManager::moveMasterStream(ParticipantId streamId,
        SyncManager *target)
    {
        auto it = mMasterStreams.find(streamId);
        if (it == mMasterStreams.end())
            std::terminate();
        if (SyncManagerResult result = target->addMasterStream(
                std::move(const_cast<BufferedInOutStream &>(*it)), false);
            result != SyncManagerResult::SUCCESS)
            return result;
        BufferedInOutStream &stream = const_cast<BufferedInOutStream &>(
            *target->mMasterStreams.find(streamId));
        std::vector<TopLevelSerializableUnitBase *> newTopLevels;
        newTopLevels.reserve(16);
        set_difference(target->getTopLevelUnits().begin(),
            target->getTopLevelUnits().end(), getTopLevelUnits().begin(),
            getTopLevelUnits().end(), back_inserter(newTopLevels));
        for (TopLevelSerializableUnitBase *newTopLevel : newTopLevels) {
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

    void SyncManager::receiveMessages(int msgCount, TimeOut timeout)
    {
        if (mSlaveStream && !mSlaveStreamInvalid) {
            if (!receiveMessages(*mSlaveStream, msgCount, timeout)) {
                removeSlaveStream();
            }
        }
        for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();) {
            if (!receiveMessages(const_cast<BufferedInOutStream &>(*it), msgCount,
                    timeout)) {
                it = mMasterStreams.erase(it);
            } else {
                ++it;
            }
        }
    }

    void SyncManager::sendMessages()
    {
        if (mSlaveStream && !mSlaveStreamInvalid) {
            if (mSlaveStream->sendMessages() == -1) {
                removeSlaveStream();
            }
        }
        for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();) {
            if (const_cast<BufferedInOutStream &>(*it).sendMessages() == -1) {
                it = mMasterStreams.erase(it);
            } else {
                ++it;
            }
        }
    }

    void SyncManager::sendAndReceiveMessages()
    {
        receiveMessages();
        sendMessages();
    }

    SerializableUnitBase *SyncManager::convertPtr(SerializeInStream &in,
        UnitId unit)
    {
        if (unit == NULL_UNIT_ID)
            return nullptr;
        try {
            if (mSlaveStream && (&in == &*mSlaveStream)) {
                return slavesMap().at(unit);
            } else {
                if (unit < RESERVED_ID_COUNT) {
                    assert(unit >= BEGIN_STATIC_ID_SPACE);
                    auto it = std::find_if(
                        mTopLevelUnits.begin(), mTopLevelUnits.end(),
                        [unit](TopLevelSerializableUnitBase *topLevel) {
                            return topLevel->masterId() == unit;
                        });
                    if (it == mTopLevelUnits.end()) {
                        throw SerializeException(
                            "Illegal TopLevel-Id used! Possible configuration "
                            "mismatch!");
                    }
                    return *it;
                } else {
                    SerializableUnitBase *u = getByMasterId(unit);
                    if (std::find(mTopLevelUnits.begin(), mTopLevelUnits.end(), u->topLevel()) == mTopLevelUnits.end()) {
                        throw SerializeException(
                            "Illegal Toplevel-Id used! Possible configuration "
                            "mismatch!");
                    }
                    return u;
                }
            }
        } catch (const std::out_of_range &) {
            std::stringstream ss;
            ss << "Unknown Unit-Id (" << unit
               << ") used! Possible binary mismatch!";
            throw SerializeException(ss.str());
        }
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

    bool SyncManager::receiveMessages(BufferedInOutStream &stream,
        int &msgCount, TimeOut timeout)
    {

        while (!stream.isClosed() && ((stream.isMessageAvailable() && msgCount == -1) || msgCount > 0)) {
            while (stream.isMessageAvailable() && msgCount != 0) {
                try {
                    readMessage(stream);
                } catch (const SerializeException &e) {
                    LOG_ERROR(e.what());
                }
                if (msgCount > 0) 
                    --msgCount;
                if (!timeout.isZero() && timeout.expired())
                    break;
            }
            if (timeout.expired())
                break;
        }

        return !stream.isClosed();
    }

    bool SyncManager::sendAllMessages(BufferedInOutStream &stream,
        TimeOut timeout)
    {
        while (int result = stream.sendMessages()) {
            if (result == -1) {
                return false;
            }
            if (timeout.expired()) {
                return false;
            }
        }
        return true;
    }

    BufferedInOutStream *SyncManager::getSlaveStream()
    {
        return mSlaveStream ? &*mSlaveStream : nullptr;
    }

    std::set<BufferedInOutStream, CompareStreamId> &SyncManager::getMasterStreams()
    {
        return mMasterStreams;
    }

    const std::set<TopLevelSerializableUnitBase *> &
    SyncManager::getTopLevelUnits() const
    {
        return mTopLevelUnits;
    }

    size_t SyncManager::clientCount() const { return mMasterStreams.size(); }

    void SyncManager::sendState(BufferedInOutStream &stream,
        SerializableUnitBase *unit)
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
