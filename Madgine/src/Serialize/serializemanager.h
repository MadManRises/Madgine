#pragma once

#include "UI\Process.h"

namespace Engine {
		namespace Serialize {

			class MADGINE_EXPORT SerializeManager {
			public:				
				SerializeManager(bool isMaster = false);
				virtual ~SerializeManager();

				void setMaster(bool b);

				void readMessage(SerializeInStream &);

				const std::map<InvPtr, SerializableUnit*> &map();

				void addMapping(InvPtr id, SerializableUnit *item);

				bool isMaster();

				UI::Process &process();

				void addBroadcastStream(SerializeOutStream *stream);
				void removeBroadcastStream(SerializeOutStream *stream);			

				bool filter(const SerializableUnit *unit, ParticipantId id);
				void addFilter(std::function<bool(const SerializableUnit*, ParticipantId)>);

				virtual std::list<SerializeOutStream*> getMessageTargets(SerializableUnit *unit);

				void addTopLevelItem(TopLevelSerializableUnit *unit);

				SerializeOutStream *getStream();

			private:
				std::map<InvPtr, SerializableUnit*> mSerializableItems;

				bool mIsMaster;

				UI::Process mProcess;

				std::list<SerializeOutStream *> mStreams;

				std::list<std::function<bool(const SerializableUnit *unit, ParticipantId id)>> mFilters;

				ParticipantId mRunningId;

				std::list<TopLevelSerializableUnit*> mTopLevelUnits;

			};

		}
}

