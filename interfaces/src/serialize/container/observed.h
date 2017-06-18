#pragma once

#include "../observable.h"
#include "serialized.h"
#include "../streams/bufferedstream.h"

namespace Engine {
		namespace Serialize {

			template <class T>
			class Observed : public Observable, public Serializable, public UnitHelper<T>{
			public:
				template <class... _Ty>
				Observed(_Ty&&... args) :
					mData(std::forward<_Ty>(args)...)
				{
					if (topLevel())
						this->setItemTopLevel(mData, topLevel());				
				}

				Observed(const Observed<T> &other) :
					mData(other.mData)
				{
					if (topLevel())
						this->setItemTopLevel(mData, topLevel());
				}

				Observed(Observed<T> &&other) :
					mData(std::forward<Observed<T>>(other))
				{
					if (topLevel())
						this->setItemTopLevel(mData, topLevel());
				}

				template <class Ty>
				void operator =(Ty&& v) {
					if (mData != v) {
						T old = mData;
						mData = std::forward<Ty>(v);
						notify(old);
					}
				}

				template <class Ty>
				void operator += (Ty&& v) {
					T old = mData;
					this->mData += std::forward<Ty>(v);
					notify(old);
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

				virtual void readRequest(BufferedInOutStream &in) override {
					T old = mData;
					this->read_state(in, this->mData);
					notify(old);
				}

				virtual void readAction(SerializeInStream &in) override {
					T old = mData;
					this->read_state(in, this->mData);
					notify(old);
				}

				virtual void readState(SerializeInStream &in) override {
					T old = mData;
					this->read_id(in, mData);
					this->read_state(in, mData);
					notify(old);
				}

				virtual void writeState(SerializeOutStream &out) const override {
					this->write_id(out, mData);
					this->write_state(out, mData);
				}

				template <class Ty>
				void setCallback(Ty &&slot) {
					mNotifySignal.connect(std::forward<Ty>(slot));
				}

				void setCondition(std::function<bool()> condition) {
					mCondition = condition;
				}

				std::function<bool()> getCondition() const {
					return mCondition;
				}

			protected:
				void notify(const T &old) {
					mNotifySignal.emit(mData, old);
					if (!mCondition || mCondition()) {
						for (BufferedOutStream *out : getMasterActionMessageTargets()) {
							this->write_state(*out, this->mData);
							out->endMessage();
						}
					}
				}

			private:
				T mData;
				SignalSlot::Signal<const T &, const T &> mNotifySignal;
				std::function<bool()> mCondition;

			};

		}
}
