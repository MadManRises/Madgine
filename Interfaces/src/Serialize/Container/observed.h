#pragma once

#include "../observable.h"
#include "serialized.h"
#include "../Streams/bufferedstream.h"


namespace Engine {
		namespace Serialize {

			template <class T>
			class Observed : public Observable, public Serialized<T>{
			public:
				Observed(SerializableUnit *parent, const T &data = T()) :
					Observable(parent),
					Serialized<T>(parent, data)
				{

				}

				template <class Ty>
				void operator =(Ty&& v) {
					if (this->mData != v) {
						this->mData = std::forward<Ty>(v);
						notify();
					}
				}

				template <class Ty>
				void operator += (Ty&& v) {
					this->mData += std::forward<Ty>(v);
					notify();
				}

				virtual void readRequest(BufferedInOutStream &in) override {
					this->read_state(in, this->mData);
					notify();
				}

				virtual void readAction(BufferedInOutStream &in) override {
					this->read_state(in, this->mData);
					notify();
				}

				void setCondition(std::function<bool()> condition) {
					mCondition = condition;
				}

				std::function<bool()> getCondition() const {
					return mCondition;
				}

			protected:
				void notify() {
					Serialized<T>::notify();
					if (!mCondition || mCondition()) {
						for (BufferedOutStream *out : getMasterActionMessageTargets()) {
							this->write_state(*out, this->mData);
							out->endMessage();
						}
					}
				}


			private:
				

				std::function<bool()> mCondition;

			};

		}
}