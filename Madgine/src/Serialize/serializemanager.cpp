#include "madginelib.h"

#include "serializemanager.h"

#include "serializablebase.h"

#include "serializableunit.h"

#include "Scene\scenemanager.h"

#include "Streams/serializestream.h"


namespace Engine {
		namespace Serialize {



			SerializeManager::SerializeManager(bool isMaster) :
				mIsMaster(isMaster),
				mRunningId(0)
			{
			}

			SerializeManager::~SerializeManager()
			{
				for (TopLevelSerializableUnit *unit : mTopLevelUnits) {
					unit->removeManager(this);
				}
			}

			void SerializeManager::setMaster(bool b)
			{
				mIsMaster = true;
			}
			
			void SerializeManager::readMessage(SerializeInStream &stream)
			{
				MessageHeader msg;
				stream.read(msg);

				SerializableUnit *object;
				if (msg.mIsMadgineComponent) {
					//object = SerializableFactoryManager::getSingleton().create(msg.mMadgineComponent, stream);
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


			std::list<SerializeOutStream*> SerializeManager::getMessageTargets(SerializableUnit * unit)
			{
				std::list<SerializeOutStream*> result;
				for (SerializeOutStream* stream : mStreams) {
					if (filter(unit, stream->id()))
						result.push_back(stream);
				}
				return result;
			}

			void SerializeManager::addTopLevelItem(TopLevelSerializableUnit * unit)
			{
				mTopLevelUnits.push_back(unit);
				unit->addManager(this);
			}

			SerializeOutStream * SerializeManager::getStream()
			{
				return mStreams.front();
			}

			bool SerializeManager::isMaster()
			{
				return mIsMaster;
			}

			UI::Process & SerializeManager::process()
			{
				return mProcess;
			}

			void SerializeManager::addBroadcastStream(SerializeOutStream * stream)
			{
				if (stream->id() != 0)
					throw 0;
				stream->setId(++mRunningId);
				mStreams.push_back(stream);
			}

			void SerializeManager::removeBroadcastStream(SerializeOutStream * stream)
			{
				stream->setId(0);
				mStreams.remove(stream);
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

		}
}
