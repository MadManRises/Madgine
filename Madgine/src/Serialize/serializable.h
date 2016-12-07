#pragma once

namespace Engine {
		namespace Serialize {


			class MADGINE_EXPORT Serializable {
			public:
				Serializable(SerializableUnit *parent);
				Serializable();

				virtual void readState(SerializeInStream&) = 0;
				virtual void writeState(SerializeOutStream&) const = 0;

				InvPtr masterId();
				void setMasterId(InvPtr id);

			private:
				InvPtr mMasterId;

			};


		}
}