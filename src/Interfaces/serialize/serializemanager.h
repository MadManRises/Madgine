#pragma once

#include "../util/process.h"

#include "../signalslot/signal.h"

#include "serializableids.h"

#include "streams/bufferedstream.h"

#include "streams/comparestreamid.h"

namespace Engine
{
	namespace Serialize
	{
		struct MessageHeader
		{
			union
			{
				MessageType mType;
				Command mCmd;
			};

			size_t mObject;
		};

		

		struct INTERFACES_EXPORT SerializeManager
		{
		public:
			SerializeManager(const std::string& name);
			SerializeManager(const SerializeManager&) = delete;
			SerializeManager(SerializeManager&&) noexcept;
			virtual ~SerializeManager();


			void readMessage(BufferedInOutStream&);

			const std::map<size_t, SerializableUnitBase*>& slavesMap() const;

			void addSlaveMapping(SerializableUnitBase* item);
			void removeSlaveMapping(SerializableUnitBase* item);

			static std::pair<size_t, SerializableUnitMap*> addStaticMasterMapping(SerializableUnitBase* item, size_t id);
			static std::pair<size_t, SerializableUnitMap*> addMasterMapping(SerializableUnitBase* item);
			static void removeMasterMapping(const std::pair<size_t, SerializableUnitMap*>&, SerializableUnitBase* item);
			static std::pair<size_t, SerializableUnitMap*> updateMasterMapping(std::pair<size_t, SerializableUnitMap*>&, SerializableUnitBase* item);

			bool isMaster(SerializeStreambuf* stream) const;
			bool isMaster() const;


			bool filter(const SerializableUnitBase* unit, ParticipantId id);
			void addFilter(std::function<bool(const SerializableUnitBase*, ParticipantId)>);

			std::set<BufferedOutStream*, CompareStreamId> getMasterMessageTargets();

			void clearTopLevelItems();
			bool addTopLevelItem(TopLevelSerializableUnitBase* unit, bool sendStateFlag = true);
			void removeTopLevelItem(TopLevelSerializableUnitBase* unit);
			void moveTopLevelItem(TopLevelSerializableUnitBase* oldUnit, TopLevelSerializableUnitBase* newUnit);

			BufferedOutStream* getSlaveMessageTarget();

			bool isMessageAvailable();
			void receiveMessages(int msgCount = -1);
			void sendMessages();
			void sendAndReceiveMessages();

			size_t convertPtr(SerializeOutStream& out, SerializableUnitBase* unit) const;
			SerializableUnitBase* convertPtr(SerializeInStream& in, size_t unit);

			std::vector<ParticipantId> getMasterParticipantIds();
			size_t clientCount() const;

			const std::string& name() const;

			SignalSlot::SignalStub<> &slaveStreamDisconnected();

		protected:


			bool receiveMessages(BufferedInOutStream &stream, int msgCount);
			bool sendAllMessages(BufferedInOutStream &stream, std::chrono::milliseconds timeout = {});

			BufferedInOutStream* getSlaveStream();

			void removeAllStreams();
			StreamError setSlaveStream(BufferedInOutStream &&stream, bool receiveState = true, std::chrono::milliseconds timeout = {});
			virtual void removeSlaveStream();
			bool addMasterStream(BufferedInOutStream &&stream, bool sendState = true);
			//virtual void removeMasterStream(BufferedInOutStream* stream);

			static ParticipantId createStreamId();

			const std::set<TopLevelSerializableUnitBase*>& getTopLevelUnits() const;

			void sendState(BufferedInOutStream& stream, TopLevelSerializableUnitBase* unit);


		private:
			SerializableUnitMap mSlaveMappings;

			bool mReceivingMasterState;

			std::set<BufferedInOutStream, CompareStreamId> mMasterStreams;
			std::optional<BufferedInOutStream> mSlaveStream;

			std::list<std::function<bool(const SerializableUnitBase*, ParticipantId)>> mFilters;

			std::set<TopLevelSerializableUnitBase*> mTopLevelUnits;

			SignalSlot::Signal<> mSlaveStreamDisconnected;

			std::string mName;

			bool mSlaveStreamInvalid;
		};
	}
}
