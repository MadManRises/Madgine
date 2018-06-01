#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../../signalslot/signal.h"
#include "unithelper.h"

namespace Engine
{
	namespace Serialize
	{
		template <class T>
		class Observed : public Observable, public Serializable, public UnitHelper<T>
		{
		public:
			template <class... _Ty>
			Observed(_Ty&&... args) :
				mData(std::forward<_Ty>(args)...)
			{
			}

			template <class Ty>
			void operator =(Ty&& v)
			{
				if (mData != v)
				{
					T old = mData;
					mData = std::forward<Ty>(v);
					notify(old);
				}
			}

			template <class Ty>
			void operator +=(Ty&& v)
			{
				T old = mData;
				this->mData += std::forward<Ty>(v);
				notify(old);
			}

			template <class Ty>
			void operator -=(Ty&& v)
			{
				T old = mData;
				this->mData -= std::forward<Ty>(v);
				notify(old);
			}

			T* operator->()
			{
				return &mData;
			}

			T* ptr()
			{
				return &mData;
			}

			operator const T &() const
			{
				return mData;
			}

			void readRequest(BufferedInOutStream& in) override
			{
				T old = mData;
				this->read_state(in, this->mData);
				notify(old);
			}

			void readAction(SerializeInStream& in) override
			{
				T old = mData;
				this->read_state(in, this->mData);
				notify(old);
			}

			void readState(SerializeInStream& in) override
			{
				T old = mData;
				this->read_id(in, mData);
				this->read_state(in, mData);
				notify(old);
			}

			void writeState(SerializeOutStream& out) const override
			{
				this->write_id(out, mData);
				this->write_state(out, mData);
			}

			template <class Ty>
			void setCallback(Ty&& slot)
			{
				mNotifySignal.connect(std::forward<Ty>(slot));
			}

			/*void setCondition(std::function<bool()> condition) {
				mCondition = condition;
			}

			std::function<bool()> getCondition() const {
				return mCondition;
			}*/

			void setActiveFlag(bool b) override
			{
				this->setItemActiveFlag(mData, b);
			}

			void notifySetActive(bool active) override
			{
				if (!active)
				{
					Serializable::notifySetActive(active);
					if (mData != T())
						mNotifySignal.emit(T(), mData);
				}
				this->notifySetItemActive(mData, active);
				if (active)
				{
					Serializable::notifySetActive(active);
					if (mData != T())
						mNotifySignal.emit(mData, T());
				}
			}

		protected:
			void notify(const T& old)
			{
				//if (!mCondition || mCondition()) {
				if (isActive())
				{
					for (BufferedOutStream* out : getMasterActionMessageTargets())
					{
						this->write_state(*out, this->mData);
						out->endMessage();
					}
				}
				if (isLocallyActive())
				{
					mNotifySignal.emit(mData, old);
				}
				//}
			}

		private:
			T mData;
			SignalSlot::Signal<const T &, const T &> mNotifySignal;
			//std::function<bool()> mCondition;
		};
	}
}
