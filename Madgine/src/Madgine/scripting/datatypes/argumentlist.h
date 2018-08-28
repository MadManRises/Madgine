#pragma once


namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT ArgumentList
		{
		public:
			ArgumentList(std::vector<ValueType> &&data);
			ArgumentList() = default;
			ArgumentList(lua_State* state, int count);

			template <class... T>
			ArgumentList(T&&... args) :
				ArgumentList(std::vector<ValueType>{ValueType(std::forward<T>(args))...})
			{
			}

			void pushToStack(lua_State* state) const;

			size_t size() const;

			ValueType &at(size_t i);
			const ValueType &at(size_t i) const;

			ValueType &front();
			const ValueType &front() const;

			bool empty() const;

			std::vector<ValueType>::iterator begin();
			std::vector<ValueType>::const_iterator begin() const;
			std::vector<ValueType>::iterator end();
			std::vector<ValueType>::const_iterator end() const;

		private:
			std::vector<ValueType> mData;
		};
	}
}
