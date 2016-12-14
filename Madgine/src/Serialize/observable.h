#pragma once



namespace Engine {
		namespace Serialize {

			class MADGINE_EXPORT Observable {
			protected:
				Observable(SerializableUnit *parent);		

				virtual void readChanges(SerializeInStream &in) = 0;

				std::list<SerializeOutStream*> getMasterMessageTargets(bool isAction);

				bool isMaster();

			private:
				SerializableUnit *mParent;
				int mIndex;


			};

		}
}