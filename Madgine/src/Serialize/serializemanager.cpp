#include "madginelib.h"

#include "serializemanager.h"

#include "serializablebase.h"

#include "serializableunit.h"

#include "Scene\scenemanager.h"

#include "Streams/bufferedstream.h"


namespace Engine {
	namespace Serialize {



		SerializeManager::SerializeManager(bool isMaster) :
			mIsMaster(isMaster),
			mRunningId(0),
			mSlaveStream(0),
			mReceivingMasterState(false)
		{
		}

		SerializeManager::~SerializeManager()
		{
			for (std::pair<const TopLevelMadgineObject, TopLevelSerializableUnit*> &unit : mTopLevelUnits) {
				unit.second->removeManager(this);
			}
		}

		bool SerializeManager::setMaster(bool b)
		{
			if (mIsMaster == b) return true;
			auto it = mTopLevelUnits.begin();
			bool success = true;
			for (; it != mTopLevelUnits.end(); ++it) {
				if (!it->second->updateManagerType(this, b)) {
					success = false;
					break;
				}
			}
			mIsMaster = b;
			if (!success) {
				for (auto it2 = mTopLevelUnits.begin(); it2 != it; ++it2) {
					assert(it->second->updateManagerType(this, !b));
				}
				mIsMaster = !b;
			}
			return success;
		}

		void SerializeManager::readMessage(BufferedInOutStream &stream)
		{
			MessageHeader msg;
			stream.read(msg);

			SerializableUnit *object;
			if (msg.mIsMadgineComponent) {
				if (msg.mMadgineComponent == SERIALIZE_MANAGER) {
					Command cmd;
					stream >> (int&)cmd;
					switch (cmd) {
					case INITIAL_STATE_DONE:
						mReceivingMasterState = false;
						break;
					default:
						throw 0;
					}
					return;
				}
				object = mTopLevelUnits.find(msg.mMadgineComponent)->second;
			}
			else {
				if (mIsMaster)
					object = reinterpret_cast<SerializableUnit*>(msg.mObject);
				else
					object = mSerializableItems.at(msg.mObject);
			}
			switch (msg.mType) {
			case ACTION:
				object->readAction(stream);
				break;
			case REQUEST:
				object->readRequest(stream);
				break;
			case STATE:
				object->readState(stream);
				object->applySerializableMap(mSerializableItems);
				break;
			default:
				throw 0;
			}
		}

		const std::map<InvPtr, SerializableUnit*>& SerializeManager::map()
		{
			return mSerializableItems;
		}

		void SerializeManager::addMapping(InvPtr id, SerializableUnit * item)
		{
			mSerializableItems[id] = item;
		}


		std::list<BufferedOutStream*> SerializeManager::getMasterMessageTargets(SerializableUnit * unit)
		{
			std::list<BufferedOutStream*> result;
			for (BufferedOutStream* stream : mMasterStreams) {
				if (filter(unit, stream->id())) {
					result.push_back(stream);
					stream->beginMessage();
				}
			}
			return result;
		}

		void SerializeManager::addTopLevelItem(TopLevelSerializableUnit * unit)
		{
			if (unit->type() == Serialize::NO_TOP_LEVEL || mTopLevelUnits.find(unit->type()) != mTopLevelUnits.end())
				throw 0;
			mTopLevelUnits[unit->type()] = unit;
			unit->addManager(this);
		}

		BufferedOutStream * SerializeManager::getSlaveMessageTarget()
		{
			mSlaveStream->beginMessage();
			return mSlaveStream;
		}

		bool SerializeManager::isMaster()
		{
			return mIsMaster;
		}

		UI::Process & SerializeManager::process()
		{
			return mProcess;
		}

		void SerializeManager::clearAllStreams()
		{
			if (mSlaveStream)
				clearSlaveStream(mSlaveStream);
			while (!mMasterStreams.empty())
				removeMasterStream(mMasterStreams.front());
		}

		bool SerializeManager::setSlaveStream(BufferedInOutStream * stream)
		{

			mReceivingMasterState = true;
			while (mReceivingMasterState) {
				if (!receiveMessages(stream))
					return false;
			}
			mSlaveStream = stream;
			return true;
		}

		void SerializeManager::clearSlaveStream(BufferedInOutStream * stream)
		{
			assert(mSlaveStream == stream);
			onSlaveStreamRemoved(stream);
			mSlaveStream = 0;
		}

		void SerializeManager::addMasterStream(BufferedInOutStream * stream)
		{
			if (stream->id() != 0)
				throw 0;
			stream->setId(++mRunningId);
			mMasterStreams.push_back(stream);
			for (const std::pair<const TopLevelMadgineObject, SerializableUnit *> &unit : mTopLevelUnits) {
				stream->beginMessage();
				unit.second->writeHeader(*stream, false);
				unit.second->writeState(*stream);
				stream->endMessage();
			}
			writeCommand(*stream, INITIAL_STATE_DONE);
		}

		void SerializeManager::removeMasterStream(BufferedInOutStream * stream)
		{
			onMasterStreamRemoved(stream);
			mMasterStreams.remove(stream);
		}

		bool SerializeManager::filter(const SerializableUnit * unit, ParticipantId id)
		{
			if (mIsMaster) {
				for (auto f : mFilters) {
					if (!f(unit, id))
						return false;
				}
			}
			return true;
		}

		void SerializeManager::addFilter(std::function<bool(const SerializableUnit*, ParticipantId)> f)
		{
			mFilters.push_back(f);
		}


		void SerializeManager::receiveMessages()
		{
			if (mSlaveStream) {
				if (!receiveMessages(mSlaveStream)) {
					onSlaveStreamRemoved(mSlaveStream);
					mSlaveStream = 0;
				}
			}
			for (auto it = mMasterStreams.begin(); it != mMasterStreams.end();) {
				if (!receiveMessages(*it)) {
					onMasterStreamRemoved(*it);
					it = mMasterStreams.erase(it);
				}
				else {
					++it;
				}
			}
		}


		bool SerializeManager::receiveMessages(BufferedInOutStream * stream)
		{
			if (!stream->isValid()) return false;


			if (stream->isMessageAvailable()) {
				readMessage(*stream);
			}

			return true;
		}

		void SerializeManager::writeCommand(BufferedOutStream &out, Command cmd)
		{
			MessageHeader header;
			header.mMadgineComponent = SERIALIZE_MANAGER;
			header.mIsMadgineComponent = true;
			out.beginMessage();
			out.write(header);
			out << (int)cmd;
			out.endMessage();
		}

		void SerializeManager::onMasterStreamRemoved(BufferedInOutStream * stream)
		{
			stream->setId(0);
		}

		void SerializeManager::onSlaveStreamRemoved(BufferedInOutStream * stream)
		{
			auto it = mSerializableItems.begin();
			while (it != mSerializableItems.end()) {
				it->second->clearMasterId();
				it = mSerializableItems.erase(it);
			}
		}


	}
}
