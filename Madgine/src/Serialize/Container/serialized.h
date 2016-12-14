#pragma once

#include "../serializable.h"
#include "../Streams/serializestream.h"

namespace Engine {
		namespace Serialize {

			template <class T>
			class Serialized : public Serializable {
			public:
				template <class... _Ty>
				Serialized(SerializableUnit *parent, _Ty&&... args) :
					mData(std::forward<_Ty>(args)...),
					Serializable(parent) {

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

				virtual void readState(SerializeInStream &in) override {
					mData.readState(in);
				}

				virtual void writeState(SerializeOutStream &out) const override {
					mData.writeState(out);
				}

			protected:
				T mData;
			};

			template <>
			class Serialized<std::string> : public Serializable {
			public:
				Serialized(SerializableUnit *parent, const std::string &data = "") :
					mData(data),
					Serializable(parent) {

				}

				template <class T>
				void operator =(T&& v) {
					mData = std::forward<T>(v);
				}

				operator const std::string &() const {
					return mData;
				}

				bool empty() const {
					return mData.empty();
				}

				void clear() {
					mData.clear();
				}

				virtual void readState(SerializeInStream &in) override {
					in >> mData;
				}

				virtual void writeState(SerializeOutStream &out) const override {
					out << mData;
				}

			protected:

				std::string mData;
			};

		}
}