#include "../interfaceslib.h"

#include "serializemanager.h"

#include "streams/bufferedstream.h"

#include "serializeexception.h"

#include "toplevelserializableunit.h"

#include "../threading/workgroup.h"

//#include <iostream>

namespace Engine
{
	namespace Serialize
	{
		Threading::WorkgroupLocal<std::map<size_t, SerializableUnitBase*>> sMasterMappings;
		size_t SerializeManager::sNextUnitId = RESERVED_ID_COUNT;
		ParticipantId SerializeManager::sRunningStreamId = sLocalMasterId;

		SerializeManager::SerializeManager(const std::string& name) :
			mReceivingMasterState(false),
			mSlaveStream(nullptr),
			mName(name),
			mSlaveStreamInvalid(false)
		{
		}

		SerializeManager::SerializeManager(SerializeManager&& other) noexcept :
			mSlaveMappings(std::forward<std::map<size_t, SerializableUnitBase*>>(other.mSlaveMappings)),
			mReceivingMasterState(other.mReceivingMasterState),
			mProcess(std::forward<Util::Process>(other.mProcess)),
			mSlaveStream(nullptr),
			mFilters(std::forward<std::list<std::function<bool(const SerializableUnitBase*, ParticipantId)>>>(other.mFilters)),
			mTopLevelUnits(std::forward<std::set<TopLevelSerializableUnitBase*>>(other.mTopLevelUnits)),
			mName(std::forward<std::string>(other.mName)),
			mSlaveStreamInvalid(false)
		{
			other.mSlaveMappings.clear();
			for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
			{
				unit->removeManager(&other);
				bool result = unit->addManager(this);
				assert(result);
			}
			other.mTopLevelUnits.clear();
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

		std::pair<size_t, SerializableUnitMap*> SerializeManager::addStaticMasterMapping(
			SerializableUnitBase* item, size_t id)
		{
			assert(id < RESERVED_ID_COUNT && id >= BEGIN_STATIC_ID_SPACE &&
				sMasterMappings->find(id) == sMasterMappings->end());
			(*sMasterMappings)[id] = item;
			//std::cout << "Master: " << id << " -> add static " << typeid(*item).name() << std::endl;
			return {id, &*sMasterMappings};
		}

		std::pair<size_t, SerializableUnitMap*> SerializeManager::addMasterMapping(SerializableUnitBase* item)
		{
			(*sMasterMappings)[sNextUnitId] = item;
			//std::cout << "Master: " << sNextUnitId << " -> add " << typeid(*item).name() << std::endl;
			return {sNextUnitId++, &*sMasterMappings};
		}

		void SerializeManager::removeMasterMapping(const std::pair<size_t, SerializableUnitMap*>& id,
		                                           SerializableUnitBase* item)
		{
			auto it = id.second->find(id.first);
			assert(it->second == item);
			id.second->erase(it);
		}

		std::pair<std::pair<size_t, SerializableUnitMap*>, std::pair<size_t, SerializableUnitMap*>> SerializeManager::
		updateMasterMapping(const std::pair<size_t, SerializableUnitMap*>& id, SerializableUnitBase* item)
		{
			assert(id.first >= RESERVED_ID_COUNT);
			if (&*sMasterMappings == id.second)
			{
				auto it = id.second->find(id.first);
				SerializableUnitBase* old = it->second;
				it->second = item;
				return {id, addMasterMapping(old)};
			}
			(*sMasterMappings)[id.first] = item;
			return {{id.first, &*sMasterMappings}, id};
		}


		std::set<BufferedOutStream*, CompareStreamId> SerializeManager::getMasterMessageTargets()
		{
			std::set<BufferedOutStream*, CompareStreamId> result;
			std::copy_if(mMasterStreams.begin(), mMasterStreams.end(), inserter(result, result.begin()),
			             [](BufferedInOutStream* stream)
			             {
				             return !stream->isClosed();
			             });
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
				for (BufferedInOutStream* stream : mMasterStreams)
				{
					this->sendState(*stream, unit);
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

		BufferedOutStream* SerializeManager::getSlaveMessageTarget() const
		{
			return mSlaveStream;
		}

		ParticipantId SerializeManager::getLocalMasterParticipantId()
		{
			return sLocalMasterId;
		}

		ParticipantId SerializeManager::getSlaveParticipantId() const
		{
			return mSlaveStream ? mSlaveStream->id() : 0;
		}

		bool SerializeManager::isMessageAvailable()
		{
			if (mSlaveStream && mSlaveStream->isMessageAvailable())
				return true;
			for (BufferedInOutStream *stream : mMasterStreams)
			{
				if (stream->isMessageAvailable())
					return true;
			}
			return false;
		}

		bool SerializeManager::isMaster(Stream* stream) const
		{
			return mSlaveStream != stream;
		}

		bool SerializeManager::isMaster() const
		{
			return mSlaveStream == nullptr || mSlaveStreamInvalid;
		}

		Util::Process& SerializeManager::process()
		{
			return mProcess;
		}

		void SerializeManager::removeAllStreams()
		{
			removeSlaveStream();
			while (!mMasterStreams.empty())
				removeMasterStream(mMasterStreams.front());
		}

		StreamError SerializeManager::setSlaveStream(BufferedInOutStream* stream, bool receiveState, std::chrono::milliseconds timeout)
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
			mSlaveStream = stream;


			if (receiveState)
			{
				if (state == NO_ERROR)
				{
					for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
					{
						unit->initSlaveId();
					}
					std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
					mReceivingMasterState = true;
					while (mReceivingMasterState)
					{
						if (timeout > std::chrono::milliseconds{} && std::chrono::duration_cast<std::chrono::milliseconds>
							(std::chrono::steady_clock::now() - start) > timeout)
						{
							state = TIMEOUT;
							mReceivingMasterState = false;
						}
						if (!receiveMessages(stream, -1))
						{
							state = stream->error();
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
				mSlaveStream = nullptr;
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
				mSlaveStream = nullptr;
				mSlaveStreamDisconnected.emit();
			}
		}

		bool SerializeManager::addMasterStream(BufferedInOutStream* stream, bool sendStateFlag)
		{
			if (sendStateFlag && *stream)
			{
				for (TopLevelSerializableUnitBase* unit : mTopLevelUnits)
				{
					sendState(*stream, unit);
				}
				stream->writeCommand(INITIAL_STATE_DONE, stream->id());
			}

			if (*stream)
			{
				mMasterStreams.push_back(stream);
				return true;
			}
			return false;
		}

		void SerializeManager::removeMasterStream(BufferedInOutStream* stream)
		{
			mMasterStreams.erase(std::remove(mMasterStreams.begin(), mMasterStreams.end(), stream), mMasterStreams.end());
		}

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


		void SerializeManager::receiveMessages(int msgCount)
		{
			if (mSlaveStream && !mSlaveStreamInvalid)
			{
				if (!receiveMessages(mSlaveStream, msgCount))
				{
					removeSlaveStream();
				}
			}
			for (size_t i = 0; i < mMasterStreams.size();)
			{
				if (!receiveMessages(mMasterStreams[i], msgCount))
				{					
					removeMasterStream(mMasterStreams[i]);
				}
				else
				{
					++i;
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
			for (size_t i = 0; i < mMasterStreams.size();)
			{
				if (mMasterStreams[i]->sendMessages() == -1)
				{
					removeMasterStream(mMasterStreams[i]);
				}
				else
				{
					++i;
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
			return unit == nullptr ? NULL_UNIT_ID : &out != mSlaveStream ? unit->masterId() : unit->slaveId();
		}

		SerializableUnitBase* SerializeManager::convertPtr(SerializeInStream& in, size_t unit)
		{
			if (unit == NULL_UNIT_ID)
				return nullptr;
			try
			{
				if (&in == mSlaveStream)
				{
					return mSlaveMappings.at(unit);
				}
				SerializableUnitBase* u = sMasterMappings->at(unit);
				if (find(mTopLevelUnits.begin(), mTopLevelUnits.end(), u->topLevel()) == mTopLevelUnits.end())
				{
					throw SerializeException("Illegal Toplevel-Id used! Possible configuration mismatch!");
				}
				return u;
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
			for (BufferedInOutStream* stream : mMasterStreams)
			{
				result.push_back(stream->id());
			}
			return result;
		}


		SignalSlot::SignalStub<>& SerializeManager::slaveStreamDisconnected()
		{
			return mSlaveStreamDisconnected;
		}

		bool SerializeManager::receiveMessages(BufferedInOutStream* stream, int msgCount)
		{
			if (stream->isClosed()) return false;


			while (stream->isMessageAvailable() && msgCount != 0)
			{
				try
				{
					readMessage(*stream);
				}
				catch (const SerializeException& e)
				{
					LOG_ERROR(e.what());
				}
				if (stream->isClosed())
					return false;
				if (msgCount > 0)
					--msgCount;
			}

			return !stream->isClosed();
		}

		bool SerializeManager::sendAllMessages(BufferedInOutStream* stream, std::chrono::milliseconds timeout)
		{
			std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			while (int result = stream->sendMessages())
			{
				if (result == -1)
				{
					return false;
				}
				if (timeout > std::chrono::milliseconds{} && std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::steady_clock::now() - start) > timeout)
				{
					return false;
				}
			}
			return true;
		}


		BufferedInOutStream* SerializeManager::getSlaveStream() const
		{
			return mSlaveStream;
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
