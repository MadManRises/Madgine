#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "../streams/bufferedstream.h"
#include "../../signalslot/signal.h"
#include "unithelper.h"
#include "offset.h"

namespace Engine
{
	namespace Serialize
	{

		#define OBSERVED(Type, Name)   \
    NO_UNIQUE_ADDRESS ::Engine::Serialize::Dummy CONCAT2(__d, __LINE__); \
    ::Engine::Serialize::Observed<::Engine::Serialize::ObservableDummyOffset<&Self::CONCAT2(__d, __LINE__), Self, alignof(Type)>, Type> Name

		template <typename PtrOffset, class T>
		class Observed : public Observable<PtrOffset>, public Serializable, public UnitHelper<T>
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
				this->read_state(in, mData);
				if (!in.isMaster())
					this->read_id(in, mData);
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

			void setDataSynced(bool b) override
			{
				this->setItemDataSynced(mData, b);
			}

			void setActive(bool active) override
			{
				if (!active)
				{
					Serializable::setActive(active);
					if (mData != T())
						mNotifySignal.emit(T(), mData);
				}
				this->setItemActive(mData, active);
				if (active)
				{
					Serializable::setActive(active);
					if (mData != T())
						mNotifySignal.emit(mData, T());
				}
			}

		protected:
			void notify(const T& old)
			{
				if (isSynced())
				{
					for (BufferedOutStream* out : this->getMasterActionMessageTargets())
					{
						this->write_state(*out, this->mData);
						out->endMessage();
					}
				}
				if (isActive())
				{
					mNotifySignal.emit(mData, old);
				}
			}

		private:
			T mData;
			SignalSlot::Signal<const T &, const T &> mNotifySignal;
		};
	}
}
