#pragma once

#include "../serializable.h"
#include "../Streams/serializestream.h"
#include "container.h"

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

				template <class T>
				void operator =(T&& v) {
					mData = std::forward<T>(v);
				}

				T *operator->() {
					return &mData;
				}

				T *ptr() {
					return &mData;
				}

				operator T &() {
					return mData;
				}

				virtual void readState(SerializeInStream &in) override {
					read_id(in, mData);
					read_state(in, mData);
				}

				virtual void writeState(SerializeOutStream &out) const override {
					write_id(out, mData);
					write_state(out, mData);
				}

			protected:
				T mData;
			};

		}
}