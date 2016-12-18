#pragma once

#include "UI\Process.h"

#include "serializablebase.h"

namespace Engine {
		namespace Serialize {

			class MADGINE_EXPORT SerializeManager {
			public:				
				SerializeManager(bool isMaster = false);
				virtual ~SerializeManager();

				bool setMaster(bool b);

				void readMessage(BufferedInOutStream &);

				const std::map<InvPtr, SerializableUnit*> &map();

				void addMapping(InvPtr id, SerializableUnit *item);

				bool isMaster();

				UI::Process &process();

				void clearAllStreams();
				bool setSlaveStream(BufferedInOutStream *stream);
				void clearSlaveStream(BufferedInOutStream *stream);
				void addMasterStream(BufferedInOutStream *stream);
				void removeMasterStream(BufferedInOutStream *stream);

				bool filter(const SerializableUnit *unit, ParticipantId id);
				void addFilter(std::function<bool(const SerializableUnit*, ParticipantId)>);

				virtual std::list<BufferedOutStream*> getMasterMessageTargets(SerializableUnit *unit);

				void addTopLevelItem(TopLevelSerializableUnit *unit);

				BufferedOutStream *getSlaveMessageTarget();

				void receiveMessages();

			protected:

				enum Command {
					INITIAL_STATE_DONE
				};

				bool receiveMessages(BufferedInOutStream *stream);

				void writeCommand(BufferedOutStream &out, Command cmd);

				virtual void onMasterStreamRemoved(BufferedInOutStream *stream);
				virtual void onSlaveStreamRemoved(BufferedInOutStream *stream);

			private:
				std::map<InvPtr, SerializableUnit*> mSerializableItems;

				bool mIsMaster;
				bool mReceivingMasterState;

				UI::Process mProcess;

				std::list<BufferedInOutStream *> mMasterStreams;
				BufferedInOutStream *mSlaveStream;

				std::list<std::function<bool(const SerializableUnit *unit, ParticipantId id)>> mFilters;

				ParticipantId mRunningId;

				std::map<TopLevelMadgineObject, TopLevelSerializableUnit*> mTopLevelUnits;



			};

		}
}

