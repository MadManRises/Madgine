#pragma once

#include "../serializable.h"
#include "unithelper.h"

namespace Engine
{
	namespace Serialize
	{
		
			template <class T>
			class Serialized : UnitHelper<T>, public SerializableBase
			{
			public:
				template <class... _Ty>
				Serialized(_Ty&&... args) :
					mData(std::forward<_Ty>(args)...)
				{
					//this->setParent(mData, unit());
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

				void readState(SerializeInStream& in)
				{
					this->read_state(in, mData);
					if (!in.isMaster())
						this->read_id(in, mData);
				}

				void writeState(SerializeOutStream& out) const
				{
					this->write_state(out, mData);
					if (out.isMaster())
						this->write_id(out, mData);
				}

				void setDataSynced(bool b)
				{
					this->setItemDataSynced(mData, b);
				}

				void setActive(bool active)
				{
					SerializableBase::setActive(active);
					this->setItemActive(mData, active);
				}

			private:
				T mData;
			};


	}
}
