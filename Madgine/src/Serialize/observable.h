#pragma once



namespace Engine {
		namespace Serialize {

			class MADGINE_EXPORT Observable {
			public:
				virtual void readRequest(BufferedInOutStream &in) = 0;
				virtual void readAction(BufferedInOutStream &in) = 0;

			protected:
				Observable(SerializableUnit *parent);		

				std::list<BufferedOutStream*> getMasterActionMessageTargets() const;
				void writeMasterActionMessageHeader(BufferedOutStream &out) const;
				BufferedOutStream *getSlaveActionMessageTarget() const;

				bool isMaster() const;

			private:
				SerializableUnit *mParent;
				int mIndex;


			};

		}
}