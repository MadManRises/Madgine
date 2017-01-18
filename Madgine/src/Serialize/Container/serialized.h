#pragma once

#include "../serializable.h"
#include "unithelper.h"
#include "../serializableunit.h"

namespace Engine {
		namespace Serialize {

			template <class T>
			class Serialized : public Serializable, protected UnitHelper<T> {
			public:
				template <class... _Ty>
				Serialized(SerializableUnit *parent, _Ty&&... args) :
					Serializable(parent),
					mData(std::forward<_Ty>(args)...)
				{
					this->setTopLevel(mData, parent->topLevel());
				}

				template <class Ty>
				void operator =(Ty&& v) {
					if (mData != v) {
						mData = std::forward<Ty>(v);
						notify();
					}
				}

				T *operator->() {
					return &mData;
				}

				T *ptr() {
					return &mData;
				}

				operator const T &() const {
					return mData;
				}

				virtual void readState(SerializeInStream &in) override {
					this->read_id(in, mData);
					this->read_state(in, mData);
					notify();
				}

				virtual void writeState(SerializeOutStream &out) const override {
					this->write_id(out, mData);
					this->write_state(out, mData);
				}


				void setCallback(std::function<void(const T &)> callback) {
					mCallback = callback;
				}

			protected:
				void notify() {
					if (mCallback)
						mCallback(mData);
				}

			protected:
				T mData;

			private:
				std::function<void(const T &)> mCallback;
			};

		}
}