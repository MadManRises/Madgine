#pragma once

#include "../serializable.h"
#include "unithelper.h"

namespace Engine
{
	namespace Serialize
	{
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

			void readState(SerializeInStream& in) override
			{
				this->read_state(in, mData);
			}

			void writeState(SerializeOutStream& out) const override
			{
				this->write_state(out, mData);
			}

			void setActiveFlag(bool b) override
			{
				this->setItemActiveFlag(mData, b);
			}

			void notifySetActive(bool active) override
			{
				Serializable::notifySetActive(active);
				this->notifySetItemActive(mData, active);
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

			T* ptr()
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

			void setActiveFlag(bool b) override
			{
				this->setItemActiveFlag(mData, b);
			}

			void notifySetActive(bool active) override
			{
				Serializable::notifySetActive(active);
				this->notifySetItemActive(mData, active);
			}

		private:
			T mData;
		};

		template <class T>
		using Serialized = typename std::conditional<std::is_base_of<SerializableUnitBase, T>::value, SerializedUnit<T>,
		                                             SerializedData<T>>::type;
	}
}
