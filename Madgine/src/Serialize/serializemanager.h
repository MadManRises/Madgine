#pragma once

#include "UI\Process.h"

#include "serializablebase.h"

namespace Engine {
		namespace Serialize {

			class MADGINE_EXPORT SerializeManager {
			public:				
				SerializeManager();
				virtual ~SerializeManager();


				void readMessage(BufferedInOutStream &);

				const std::map<InvPtr, SerializableUnit*> &map();

				void addMapping(InvPtr id, SerializableUnit *item);

				bool isMaster();

				UI::Process &process();

				void clearAllStreams();
				bool setSlaveStream(BufferedInOutStream *stream);
				void clearSlaveStream();
				void addMasterStream(BufferedInOutStream *stream);
				void removeMasterStream(BufferedInOutStream *stream);

				bool filter(const SerializableUnit *unit, ParticipantId id);
				void addFilter(std::function<bool(const SerializableUnit*, ParticipantId)>);

				virtual std::list<BufferedOutStream*> getMasterMessageTargets(SerializableUnit *unit);

				void addTopLevelItem(TopLevelSerializableUnit *unit);

				BufferedOutStream *getSlaveMessageTarget();

				void receiveMessages();

				SerializableUnit *readPtr(SerializeInStream &in);
				InvPtr convertPtr(SerializeOutStream &out, SerializableUnit *unit);
				SerializableUnit *convertPtr(SerializeInStream &in, InvPtr unit);
				void writePtr(SerializeOutStream &out, SerializableUnit *unit);

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

