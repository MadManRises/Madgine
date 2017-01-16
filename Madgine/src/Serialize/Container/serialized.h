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
					mData(std::forward<_Ty>(args)...),
					Serializable(parent) {
					setTopLevel(mData, parent->topLevel());
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
					read_id(in, mData);
					read_state(in, mData);
					notify();
				}

				virtual void writeState(SerializeOutStream &out) const override {
					write_id(out, mData);
					write_state(out, mData);
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