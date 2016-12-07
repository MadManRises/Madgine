#pragma once



namespace Engine {
		namespace Serialize {

			class Observable {
			protected:
				Observable(SerializableUnit *parent);		

				virtual void readChanges(SerializeInStream &in) = 0;

				std::list<SerializeOutStream*> getMessageTargets(bool isAction);

			private:
				SerializableUnit *mParent;
				int mIndex;


			};

		}
}