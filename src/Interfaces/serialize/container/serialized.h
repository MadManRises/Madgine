#pragma once

#include "../serializable.h"
#include "unithelper.h"

namespace Engine
{
	namespace Serialize
	{
		
		namespace __serialized__impl__{

			template <class T>
			class SerializedData : UnitHelper<T>, public Serializable
			{
			public:
				template <class... _Ty>
				SerializedData(_Ty&&... args) :
					mData(std::forward<_Ty>(args)...)
				{
				}

				template <class Ty>
				void operator =(Ty&& v)
				{
					mData = std::forward<Ty>(v);
				}

				T& operator*()
				{
					return mData;
				}
				 
				const T& operator*() const
				{
					return mData;
				}

				T* operator->()
				{
					return &mData;
				}

				const T* operator->() const
				{
					return &mData;
				}

				T* ptr()
				{
					return &mData;
				}

				const T* ptr() const
				{
					return &mData;
				}

				operator const T &() const
				{
					return mData;
				}

				void readState(SerializeInStream& in) override
				{
					this->read_state(in, mData);
				}

				void writeState(SerializeOutStream& out) const override
				{
					this->write_state(out, mData);
				}

				void setDataSynced(bool b) override
				{
					this->setItemDataSynced(mData, b);
				}

				void setActive(bool active) override
				{
					Serializable::setActive(active);
					this->setItemActive(mData, active);
				}

			private:
				T mData;
			};


			template <class T>
			class SerializedUnit : UnitHelper<T>, public Serializable
			{
			public:
				template <class... _Ty>
				SerializedUnit(_Ty&&... args) :
					mData(std::forward<_Ty>(args)...)
				{
					mData.postConstruct();
					mData.SerializableUnitBase::setParent(unit());
				}

				/*template <class Ty>
				void operator =(Ty&& v) {
				if (mData != v) {
				mData = std::forward<Ty>(v);
				notify();
				}
				}*/

				T* operator->()
				{
					return &mData;
				}

				const T* operator->() const
				{
					return &mData;
				}

				T* ptr()
				{
					return &mData;
				}

				const T* ptr() const
				{
					return &mData;
				}

				operator const T &() const
				{
					return mData;
				}

				void readState(SerializeInStream& in) override
				{
					this->read_id(in, mData);
					this->read_state(in, mData);
				}

				void writeState(SerializeOutStream& out) const override
				{
					this->write_id(out, mData);
					this->write_state(out, mData);
				}

				void setDataSynced(bool b) override
				{
					this->setItemDataSynced(mData, b);
				}

				void setActive(bool active) override
				{
					Serializable::setActive(active);
					this->setItemActive(mData, active);
				}

			private:
				T mData;
			};
		}

		template <class T>
		using Serialized = typename std::conditional<std::is_base_of<SerializableUnitBase, T>::value, __serialized__impl__::SerializedUnit<T>,
		                                             __serialized__impl__::SerializedData<T>>::type;
	}
}
