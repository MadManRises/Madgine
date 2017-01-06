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
					Serialized(parent, data)
				{

				}

				template <class T>
				void operator =(T&& v) {
					if (mData != v) {
						mData = std::forward<T>(v);
						notify();
					}
				}

				template <class T>
				void operator += (T&& v) {
					mData += std::forward<T>(v);
					notify();
				}

				virtual void readRequest(BufferedInOutStream &in) override {
					read_state(in, mData);
					notify();
				}

				virtual void readAction(BufferedInOutStream &in) override {
					read_state(in, mData);
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
					Serialized::notify();
					if (!mCondition || mCondition()) {
						for (BufferedOutStream *out : getMasterActionMessageTargets()) {
							write_state(*out, mData);
							out->endMessage();
						}
					}
				}


			private:
				

				std::function<bool()> mCondition;

			};

		}
}