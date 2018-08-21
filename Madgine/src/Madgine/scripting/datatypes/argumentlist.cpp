#include "../../interfaceslib.h"
#include "argumentlist.h"
#include "../../serialize/streams/serializestream.h"
#include "../types/apihelper.h"


namespace Engine
{
	namespace Scripting
	{
		ArgumentList::ArgumentList(std::vector<ValueType>&& data) :
		mData(std::forward<std::vector<ValueType>>(data))
		{
		}

		ArgumentList::ArgumentList(lua_State* state, int count)
		{
			mData.resize(count);
			for (int i = 0; i < count; ++i)
			{
				mData.at(i) = ValueType::fromStack(state, i - count);
			}
			APIHelper::pop(state, count);
		}

		void ArgumentList::readState(Serialize::SerializeInStream& in)
		{
			mData.clear();
			ValueType temp;
			while (in.loopRead(temp))
			{
				mData.emplace_back(std::move(temp));
			}
		}

		void ArgumentList::writeState(Serialize::SerializeOutStream& out) const
		{
			for (const ValueType& v : mData)
			{
				out << v;
			}
			out << ValueType::EOL();
		}

		void ArgumentList::pushToStack(lua_State* state) const
		{
			for (const ValueType& v : mData)
			{
				v.push(state);
			}
		}

		size_t ArgumentList::size() const
		{
			return mData.size();
		}

		ValueType& ArgumentList::at(size_t i)
		{
			return mData.at(i);
		}

		const ValueType &ArgumentList::at(size_t i) const
		{
			return mData.at(i);
		}

		ValueType& ArgumentList::front()
		{
			return mData.front();
		}

		const ValueType &ArgumentList::front() const
		{
			return mData.front();
		}

		bool ArgumentList::empty() const
		{
			return mData.empty();
		}

		std::vector<ValueType>::iterator ArgumentList::begin()
		{
			return mData.begin();
		}

		std::vector<ValueType>::const_iterator ArgumentList::begin() const
		{
			return mData.begin();
		}

		std::vector<ValueType>::iterator ArgumentList::end()
		{
			return mData.end();
		}

		std::vector<ValueType>::const_iterator ArgumentList::end() const
		{
			return mData.end();
		}
	}
}
