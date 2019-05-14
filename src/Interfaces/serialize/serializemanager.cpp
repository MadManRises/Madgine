#include "../interfaceslib.h"

#include "serializemanager.h"

#include "streams/bufferedstream.h"

#include "serializeexception.h"

#include "toplevelserializableunit.h"

#include "../threading/workgroup.h"

#include "../serialize/streams/buffered_streambuf.h"

#include "../generic/transformIt.h"

//#include <iostream>

namespace Engine
{
	namespace Serialize
	{
		std::mutex sMasterMappingMutex;
		std::map<size_t, SerializableUnitBase*> sMasterMappings;
		size_t sNextUnitId = RESERVED_ID_COUNT;
        static ParticipantId sLocalMasterParticipantId = 1;
		std::atomic<ParticipantId> sRunningStreamId = sLocalMasterParticipantId;

		SerializeManager::SerializeManager(const std::string& name) :
			mReceivingMasterState(false),
			mName(name),
			mSlaveStreamInvalid(false)
		{
		}

		SerializeManager::SerializeManager(SerializeManager&& other) noexcept :
			mSlaveMappings(std::move(other.mSlaveMappings)),
			mReceivingMasterState(other.mReceivingMasterState),
			mFilters(std::move(other.mFilters)),
			mTopLevelUnits(std::move(other.mTopLevelUnits)),
			mName(std::move(other.mName)),
			mSlaveStreamInvalid(other.mSlaveStreamInvalid)
		{
			other.mSlaveMappings.clear();
			for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
			{
				unit->removeManager(&other);
				bool result = unit->addManager(this);
				assert(result);
			}
			other.mTopLevelUnits.clear();
			if (other.mSlaveStream)
			{
				mSlaveStream.emplace(std::move(*other.mSlaveStream));
				other.mSlaveStream.reset();
				mSlaveStream->setManager(*this);
			}
			mMasterStreams = std::move(other.mMasterStreams);
			other.mMasterStreams.clear();
			for (const BufferedInOutStream &stream : mMasterStreams)
			{
				stream.setManager(*this);
			}
		}

		SerializeManager::~SerializeManager()
		{
			clearTopLevelItems();
		}


		void SerializeManager::readMessage(BufferedInOutStream& stream)
		{
			MessageHeader header;
			stream.readHeader(header);

			if (header.mObject == SERIALIZE_MANAGER)
			{
				stream.logReadHeader(header, mName);
				ParticipantId id;
				switch (header.mCmd)
				{
				case INITIAL_STATE_DONE:
					mReceivingMasterState = false;
					stream >> id;
					stream.setId(id);
					break;
				case STREAM_EOF:
					stream.close();
					break;
				default:
					throw SerializeException("Unknown Builtin Command-Code for SerializeManager: " + std::to_string(header.mCmd));
				}
			}
			else
			{
				SerializableUnitBase* object = convertPtr(stream, header.mObject);
				stream.logReadHeader(header, typeid(*object).name());
				switch (header.mType)
				{
				case ACTION:
					object->readAction(stream);
					break;
				case REQUEST:
					object->readRequest(stream);
					break;
				case STATE:
					object->readState(stream);
					object->applySerializableMap(mSlaveMappings);
					break;
				default:
					throw SerializeException("Invalid Message-Type: " + std::to_string(header.mType));
				}
			}
		}

		const std::map<size_t, SerializableUnitBase*>& SerializeManager::slavesMap() const
		{
			return mSlaveMappings;
		}

		void SerializeManager::addSlaveMapping(SerializableUnitBase* item)
		{
			assert(mSlaveMappings.find(item->slaveId()) == mSlaveMappings.end());
			mSlaveMappings[item->slaveId()] = item;
			//std::cout << "Slave: " << item->slaveId() << " -> add " << typeid(*item).name() << std::endl;
		}

		void SerializeManager::removeSlaveMapping(SerializableUnitBase* item)
		{
			size_t result = mSlaveMappings.erase(item->slaveId());
			assert(result == 1);
		}

		size_t SerializeManager::generateMasterId(size_t id, SerializableUnitBase * unit)
		{
			if (id == 0 || id >= RESERVED_ID_COUNT)
			{
				std::lock_guard guard(sMasterMappingMutex);
				id = ++sNextUnitId;
				sMasterMappings[id] = unit;
				//std::cout << "Master: " << sNextUnitId << " -> add " << typeid(*item).name() << std::endl;
				return id;
			}
			else
			{
				assert(id >= BEGIN_STATIC_ID_SPACE);
				//std::cout << "Master: " << id << " -> add static " << typeid(*item).name() << std::endl;
				return id;
			}
		}

		void SerializeManager::deleteMasterId(size_t id, SerializableUnitBase * unit)
		{
			if (id >= RESERVED_ID_COUNT)
			{
				std::lock_guard guard(sMasterMappingMutex);
				auto it = sMasterMappings.find(id);
				assert(it->second == unit);
				sMasterMappings.erase(it);
			}
			else
			{
				assert(id >= BEGIN_STATIC_ID_SPACE);
			}
		}

		std::set<BufferedOutStream*, CompareStreamId> SerializeManager::getMasterMessageTargets()
		{
			std::set<BufferedOutStream*, CompareStreamId> result;

			for (const BufferedInOutStream &stream : mMasterStreams)
			{
				if (!stream.isClosed())
				{
					result.insert(const_cast<BufferedInOutStream*>(&stream));
				}
			}
			return result;
		}


		void SerializeManager::clearTopLevelItems()
		{
			while (!mSlaveMappings.empty())
			{
				mSlaveMappings.begin()->second->clearSlaveId();
			}
			for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
			{
				unit->removeManager(this);
			}
			mTopLevelUnits.clear();
		}

		bool SerializeManager::addTopLevelItem(TopLevelSerializableUnitBase* unit, bool sendStateFlag)
		{
			if (!unit->addManager(this))
				return false;
			mTopLevelUnits.insert(unit);

			if (mSlaveStream)
			{
				unit->initSlaveId();
			}

			if (sendStateFlag && unit->isSynced())
			{
				for (const BufferedInOutStream &stream : mMasterStreams)
				{
					this->sendState(const_cast<BufferedInOutStream&>(stream), unit);
				}
			}
			return true;
		}

		void SerializeManager::removeTopLevelItem(TopLevelSerializableUnitBase* unit)
		{
			auto it2 = mSlaveMappings.begin();
			while (it2 != mSlaveMappings.end())
			{
				if (it2->second->topLevel() == unit)
				{
					it2++->second->clearSlaveId();
				}
				else
				{
					++it2;
				}
			}
			unit->removeManager(this);
			mTopLevelUnits.erase(unit);
		}

		void SerializeManager::moveTopLevelItem(TopLevelSerializableUnitBase* oldUnit, TopLevelSerializableUnitBase* newUnit)
		{
			removeTopLevelItem(oldUnit);
			addTopLevelItem(newUnit, false);
		}

		BufferedOutStream* SerializeManager::getSlaveMessageTarget()
		{
			return &*mSlaveStream;
		}

		bool SerializeManager::isMessageAvailable()
		{
			if (mSlaveStream && mSlaveStream->isMessageAvailable())
				return true;
			for (const BufferedInOutStream &stream : mMasterStreams)
			{
				if (stream.isMessageAvailable())
					return true;
			}
			return false;
		}

		bool SerializeManager::isMaster(SerializeStreambuf* stream) const
		{
			return !mSlaveStream || &mSlaveStream->buffer() != stream;
		}

		bool SerializeManager::isMaster() const
		{
			return !mSlaveStream || mSlaveStreamInvalid;
		}

		void SerializeManager::removeAllStreams()
		{
			removeSlaveStream();
			mMasterStreams.clear();
		}

		StreamError SerializeManager::setSlaveStream(BufferedInOutStream &&stream, bool receiveState, TimeOut timeout)
		{
			if (mSlaveStream)
				return UNKNOWN_ERROR;

			StreamError state = NO_ERROR;

			auto it = mTopLevelUnits.begin();

			for (; it != mTopLevelUnits.end(); ++it)
			{
				if (!(*it)->updateManagerType(this, false))
				{
					state = UNKNOWN_ERROR;
					break;
				}
			}

			mSlaveStreamInvalid = true;
			mSlaveStream.emplace(std::move(stream));


			if (receiveState)
			{
				if (state == NO_ERROR)
				{
					for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
					{
						unit->initSlaveId();
					}
					mReceivingMasterState = true;
					while (mReceivingMasterState)
					{
						int msgCount = -1;
						if (!receiveMessages(*mSlaveStream, msgCount))
						{
							state = mSlaveStream->error();
							mReceivingMasterState = false;
						}
						if (mReceivingMasterState && timeout.expired())
						{
							state = TIMEOUT;
							mReceivingMasterState = false;
						}
					}
					if (state != NO_ERROR)
					{
						while (!mSlaveMappings.empty())
						{
							mSlaveMappings.begin()->second->clearSlaveId();
						}
					}
				}
			}

			if (state != NO_ERROR)
			{
				mSlaveStreamInvalid = false;
				for (auto it2 = mTopLevelUnits.begin(); it2 != it; ++it2)
				{
					bool result = (*it2)->updateManagerType(this, true);
					assert(result);
				}
				mSlaveStream.reset();
			}

			mSlaveStreamInvalid = false;

			return state;
		}

		void SerializeManager::removeSlaveStream()
		{
			if (mSlaveStream)
			{
				while (!mSlaveMappings.empty())
				{
					size_t s = mSlaveMappings.size();
					mSlaveMappings.begin()->second->clearSlaveId();
					assert(s > mSlaveMappings.size());
				}
				for (TopLevelSerializableUnitBase* topLevel : mTopLevelUnits)
				{
					topLevel->updateManagerType(this, true);
				}
				mSlaveStream->setId(0);
				mSlaveStream.reset();
				mSlaveStreamDisconnected.emit();
			}
		}

		bool SerializeManager::addMasterStream(BufferedInOutStream &&stream, bool sendStateFlag)
		{
			if (sendStateFlag && stream)
			{
				for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
				{
					sendState(stream, unit);
				}
				stream.writeCommand(INITIAL_STATE_DONE, stream.id());
			}

			if (stream)
			{
				mMasterStreams.emplace(std::move(stream));
				return true;
			}
			return false;
		}
		/*
		void SerializeManager::removeMasterStream(BufferedInOutStream* stream)
		{
			mMasterStreams.erase(std::remove(mMasterStreams.begin(), mMasterStreams.end(), stream), mMasterStreams.end());
		}*/

		bool SerializeManager::filter(const SerializableUnitBase* unit, ParticipantId id)
		{
			for (auto& f : mFilters)
			{
				if (!f(unit, id))
				{
					return false;
				}
			}
			return true;
		}

		void SerializeManager::addFilter(std::function<bool(const SerializableUnitBase*, ParticipantId)> f)
		{
			mFilters.push_back(f);
		}


		void SerializeManager::receiveMessages(int msgCount, TimeOut timeout)
		{
			if (mSlaveStream && !mSlaveStreamInvalid)
			{
				if (!receiveMessages(*mSlaveStream, msgCount, timeout))
				{
					removeSlaveStream();
				}
			}
			for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();)
			{
				if (!receiveMessages(const_cast<BufferedInOutStream&>(*it), msgCount, timeout))
				{					
					it = mMasterStreams.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void SerializeManager::sendMessages()
		{
			if (mSlaveStream && !mSlaveStreamInvalid)
			{
				if (mSlaveStream->sendMessages() == -1)
				{
					removeSlaveStream();
				}
			}
			for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();)
			{
				if (const_cast<BufferedInOutStream&>(*it).sendMessages() == -1)
				{
					it = mMasterStreams.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void SerializeManager::sendAndReceiveMessages()
		{
			receiveMessages();
			sendMessages();
		}

		size_t SerializeManager::convertPtr(SerializeOutStream& out, SerializableUnitBase* unit) const
		{
			return unit == nullptr ? NULL_UNIT_ID : (!mSlaveStream || &out != &*mSlaveStream) ? unit->masterId() : unit->slaveId();
		}

		SerializableUnitBase* SerializeManager::convertPtr(SerializeInStream& in, size_t unit)
		{
			if (unit == NULL_UNIT_ID)
				return nullptr;
			try
			{
				if (mSlaveStream && (&in == &*mSlaveStream))
				{
					return mSlaveMappings.at(unit);
				}
				else
				{
					if (unit < RESERVED_ID_COUNT)
					{
						assert(unit >= BEGIN_STATIC_ID_SPACE);
						auto it = std::find_if(mTopLevelUnits.begin(), mTopLevelUnits.end(), [unit](TopLevelSerializableUnitBase *topLevel)
						{
							return topLevel->masterId() == unit;
						});
						if (it == mTopLevelUnits.end())
						{
							throw SerializeException("Illegal TopLevel-Id used! Possible configuration mismatch!");
						}
						return *it;
					}
					else
					{
						SerializableUnitBase* u;
						{
							std::lock_guard guard(sMasterMappingMutex);
							u = sMasterMappings.at(unit);
						}
						if (find(mTopLevelUnits.begin(), mTopLevelUnits.end(), u->topLevel()) == mTopLevelUnits.end())
						{
							throw SerializeException("Illegal Toplevel-Id used! Possible configuration mismatch!");
						}
						return u;
					}
				}
			}
			catch (const std::out_of_range&)
			{
				std::stringstream ss;
				ss << "Unknown Unit-Id (" << unit << ") used! Possible binary mismatch!";
				throw SerializeException(ss.str());
			}
		}

		std::vector<ParticipantId> SerializeManager::getMasterParticipantIds()
		{
			std::vector<ParticipantId> result;
			result.reserve(mMasterStreams.size());
			for (const BufferedInOutStream &stream : mMasterStreams)
			{
				result.push_back(stream.id());
			}
			return result;
		}


		SignalSlot::SignalStub<>& SerializeManager::slaveStreamDisconnected()
		{
			return mSlaveStreamDisconnected;
        }

        ParticipantId SerializeManager::getParticipantId(SerializeManager *manager) {
			if (manager)
			{
                assert(manager->mSlaveStream);
                return manager->mSlaveStream->id();
			}
			else
			{
                return sLocalMasterParticipantId;
			}
        }

		bool SerializeManager::receiveMessages(BufferedInOutStream &stream, int &msgCount, TimeOut timeout)
		{

			while (!stream.isClosed() && ((stream.isMessageAvailable() && msgCount == -1) || msgCount > 0))
			{
				while (stream.isMessageAvailable() && msgCount != 0)
				{
					try
					{
						readMessage(stream);
					}
					catch (const SerializeException& e)
					{
						LOG_ERROR(e.what());
					}
					if (msgCount > 0)
					{
						--msgCount;
					}
					if (!timeout.isZero() && timeout.expired())
						break;
				}
				if (timeout.expired())
					break;
			}

			return !stream.isClosed();
		}

		bool SerializeManager::sendAllMessages(BufferedInOutStream &stream, TimeOut timeout)
		{
			while (int result = stream.sendMessages())
			{
				if (result == -1)
				{
					return false;
				}
				if (timeout.expired())
				{
					return false;
				}
			}
			return true;
		}


		BufferedInOutStream* SerializeManager::getSlaveStream()
		{
			return mSlaveStream ? &*mSlaveStream : nullptr;
		}


		ParticipantId SerializeManager::createStreamId()
		{
			return ++sRunningStreamId;
		}

		const std::set<TopLevelSerializableUnitBase*>& SerializeManager::getTopLevelUnits() const
		{
			return mTopLevelUnits;
		}

		size_t SerializeManager::clientCount() const
		{
			return mMasterStreams.size();
		}


		void SerializeManager::sendState(BufferedInOutStream& stream, TopLevelSerializableUnitBase* unit)
		{
			stream.beginMessage(unit, STATE);
			unit->writeState(stream);
			stream.endMessage();
		}

		const std::string& SerializeManager::name() const
		{
			return mName;
		}
	}
}
