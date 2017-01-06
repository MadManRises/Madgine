#include "madginelib.h"

#include "serializemanager.h"

#include "serializablebase.h"

#include "serializableunit.h"

#include "Scene\scenemanager.h"

#include "Streams/bufferedstream.h"


namespace Engine {
	namespace Serialize {



		SerializeManager::SerializeManager() :
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
				auto it = mTopLevelUnits.find(msg.mMadgineComponent);
				if (it == mTopLevelUnits.end())
					throw 0;
				object = it->second;
			}
			else {
				object = convertPtr(stream, msg.mObject);
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
			for (BufferedInOutStream* stream : mMasterStreams) {
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

			MessageHeader header;
			header.mType = STATE;
			header.mIsMadgineComponent = true;
			header.mMadgineComponent = unit->type();

			for (BufferedInOutStream *stream : mMasterStreams) {
				stream->beginMessage();

				stream->write(header);
				unit->writeState(*stream);
				stream->endMessage();
			}
		}

		void SerializeManager::removeTopLevelItem(TopLevelSerializableUnit * unit)
		{
			unit->removeManager(this);
			auto it = std::find_if(mTopLevelUnits.begin(), mTopLevelUnits.end(), [=](const std::pair<const TopLevelMadgineObject, TopLevelSerializableUnit*> &p) {return p.second == unit; });
			if (it == mTopLevelUnits.end())
				throw 0;
			mTopLevelUnits.erase(it);
		}

		BufferedOutStream * SerializeManager::getSlaveMessageTarget()
		{
			mSlaveStream->beginMessage();
			return mSlaveStream;
		}

		bool SerializeManager::isMaster()
		{
			return mSlaveStream == 0;
		}

		UI::Process & SerializeManager::process()
		{
			return mProcess;
		}

		void SerializeManager::clearAllStreams()
		{
			clearSlaveStream();
			while (!mMasterStreams.empty())
				removeMasterStream(mMasterStreams.front());
		}

		bool SerializeManager::setSlaveStream(BufferedInOutStream * stream)
		{
			if (mSlaveStream)
				return false;

			auto it = mTopLevelUnits.begin();
			bool success = true;
			for (; it != mTopLevelUnits.end(); ++it) {
				if (!it->second->updateManagerType(this, false)) {
					success = false;
					break;
				}
			}

			if (success) {
				mReceivingMasterState = true;
				while (mReceivingMasterState) {
					if (!receiveMessages(stream)) {
						success = false;
						mReceivingMasterState = false;
					}
				}
			}

			if (!success) {
				for (auto it2 = mTopLevelUnits.begin(); it2 != it; ++it2) {
					assert(it->second->updateManagerType(this, true));
				}
			}
			else {
				mSlaveStream = stream;
			}
			return success;
		}

		void SerializeManager::clearSlaveStream()
		{
			if (mSlaveStream)
				onSlaveStreamRemoved(mSlaveStream);
		}

		void SerializeManager::addMasterStream(BufferedInOutStream * stream)
		{
			if (stream->id() != 0)
				throw 0;
			stream->setId(++mRunningId);

			MessageHeader header;
			header.mType = STATE;
			header.mIsMadgineComponent = true;

			mMasterStreams.push_back(stream);
			for (const std::pair<const TopLevelMadgineObject, SerializableUnit *> &unit : mTopLevelUnits) {
				stream->beginMessage();
				
				header.mMadgineComponent = unit.second->type();				
				stream->write(header);

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
			for (auto f : mFilters) {
				if (!f(unit, id)){
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
					clearSlaveStream();
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

		SerializableUnit * SerializeManager::readPtr(SerializeInStream & in)
		{
			InvPtr ptr;
			in >> ptr;
			return convertPtr(in, ptr);
		}

		void SerializeManager::writePtr(SerializeOutStream & out, SerializableUnit * unit)
		{
			out << convertPtr(out, unit);
		}


		InvPtr SerializeManager::convertPtr(SerializeOutStream & out, SerializableUnit * unit)
		{
			return &out != mSlaveStream ? static_cast<InvPtr>(reinterpret_cast<uintptr_t>(unit)) : unit->masterId();
		}

		SerializableUnit * SerializeManager::convertPtr(SerializeInStream & in, InvPtr unit)
		{
			return &in != mSlaveStream ? reinterpret_cast<SerializableUnit*>(unit) : mSerializableItems.at(unit);
		}


		bool SerializeManager::receiveMessages(BufferedInOutStream * stream)
		{
			if (!stream->isValid()) return false;


 			while (stream->isMessageAvailable()) {
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
			assert(mSlaveStream == stream);
			auto it = mSerializableItems.begin();
			while (it != mSerializableItems.end()) {
				it->second->clearMasterId();
				it = mSerializableItems.erase(it);
			}
			for (const std::pair<const TopLevelMadgineObject, TopLevelSerializableUnit *> &topLevel : mTopLevelUnits) {
				topLevel.second->updateManagerType(this, true);
			}
			mSlaveStream = 0;
		}


	}
}
