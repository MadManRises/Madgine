#pragma once

#include "util/process.h"

#include "signalslot/signal.h"

namespace Engine {
		namespace Serialize {




			enum Command {
				INITIAL_STATE_DONE,
				STREAM_EOF
			};

			struct MessageHeader {
				union {
					MessageType mType;
					Command mCmd;
				};
				size_t mObject;
			};

			enum ReservedIds {
				NO_ID = 0,
				UNINITIALIZED = 1,
				SERIALIZE_MANAGER = 1,
				BEGIN_USER_ID_SPACE,
				RESERVED_ID_COUNT = 256
			};

			class INTERFACES_EXPORT SerializeManager {
			public:				
				SerializeManager(const std::string &name);
				SerializeManager(const SerializeManager &) = delete;
				SerializeManager(SerializeManager &&);
				virtual ~SerializeManager();


				void readMessage(BufferedInOutStream &);

				const std::map<size_t, SerializableUnitBase*> &slavesMap();

				void addSlaveMapping(SerializableUnitBase *item);
				void removeSlaveMapping(SerializableUnitBase *item);

				static std::pair<size_t, SerializableUnitMap*> addStaticMasterMapping(SerializableUnitBase *item, size_t id);
				static std::pair<size_t, SerializableUnitMap*> addMasterMapping(SerializableUnitBase *item);
				static void removeMasterMapping(const std::pair<size_t, SerializableUnitMap*> &, SerializableUnitBase * item);
				static std::pair<std::pair<size_t, SerializableUnitMap*>, std::pair<size_t, SerializableUnitMap*>> updateMasterMapping(const std::pair<size_t, SerializableUnitMap*> &, SerializableUnitBase * item);

				bool isMaster(Stream *stream);
				bool isMaster();

				Engine::Util::Process &process();


				bool filter(const SerializableUnitBase *unit, ParticipantId id);
				void addFilter(std::function<bool(const SerializableUnitBase*, ParticipantId)>);

				virtual std::list<BufferedOutStream*> getMasterMessageTargets(SerializableUnitBase *unit, MessageType type, const std::function<bool(SerializableUnitBase*, ParticipantId)> &customFilter = {});

				void clearTopLevelItems();
				bool addTopLevelItem(TopLevelSerializableUnitBase *unit, bool sendStateFlag = true);
				void removeTopLevelItem(TopLevelSerializableUnitBase *unit);
				void moveTopLevelItem(TopLevelSerializableUnitBase *oldUnit, TopLevelSerializableUnitBase *newUnit);

				BufferedOutStream *getSlaveMessageTarget(SerializableUnitBase *unit);
				static ParticipantId getLocalMasterParticipantId();
				ParticipantId getSlaveParticipantId();

				void receiveMessages();
				void sendMessages();
				void sendAndReceiveMessages();

				size_t convertPtr(SerializeOutStream &out, SerializableUnitBase *unit);
				SerializableUnitBase *convertPtr(SerializeInStream &in, size_t unit);

				template <class T>
				void connectSlaveStreamDisconnected(T &slot) {
					mSlaveStreamDisconnected.connect(slot);
				}

				std::vector<ParticipantId> getMasterParticipantIds();

				const std::string &name() const;

			protected:


				bool receiveMessages(BufferedInOutStream *stream);
				bool sendAllMessages(BufferedInOutStream *stream, int timeout = 0);
				
				BufferedInOutStream *getSlaveStream();

				void removeAllStreams();
				StreamError setSlaveStream(BufferedInOutStream *stream, bool receiveState = true, int timeout = 0);
				virtual void removeSlaveStream();
				bool addMasterStream(BufferedInOutStream *stream, bool sendState = true);
				virtual void removeMasterStream(BufferedInOutStream *stream);

				static ParticipantId createStreamId();

				const std::set<TopLevelSerializableUnitBase*> &getTopLevelUnits();

				void sendState(BufferedInOutStream &stream, TopLevelSerializableUnitBase *unit);


			private:
				SerializableUnitMap mSlaveMappings;
				struct intern {
					static thread_local SerializableUnitMap sMasterMappings;
				};
				static size_t sNextUnitId;

				bool mReceivingMasterState;

				Engine::Util::Process mProcess;

				std::list<BufferedInOutStream *> mMasterStreams;
				BufferedInOutStream *mSlaveStream;

				std::list<std::function<bool(const SerializableUnitBase *, ParticipantId)>> mFilters;

				static constexpr ParticipantId sLocalMasterId = 1;
				static ParticipantId sRunningStreamId;

				std::set<TopLevelSerializableUnitBase*> mTopLevelUnits;

				SignalSlot::Signal<> mSlaveStreamDisconnected;

				std::string mName;

			};

		}
}

