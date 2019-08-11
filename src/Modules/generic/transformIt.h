#pragma once

namespace Engine
{
	
	
	template <class T, typename Converter>
	class TransformItContainer : public T
	{
	private:
		TransformItContainer() = delete;
		TransformItContainer(const TransformItContainer<T, Converter>&) = delete;
		TransformItContainer(TransformItContainer<T, Converter>&&) = delete;

		template <class It>
		class TransformIterator
		{
		public:

			using iterator_category = typename It::iterator_category;
			using value_type = typename std::remove_reference<decltype(std::declval<Converter>()(std::declval<typename It::reference>()))>::type;
			using difference_type = ptrdiff_t;
			using pointer = void;
			using reference = value_type;

		
			TransformIterator(It&& it) :
				mIt(std::forward<It>(it))
			{
			}

			void operator++()
			{
				++mIt;
			}

			decltype(auto) operator*() const
			{
				return Converter{}(*mIt);
			}

			bool operator!=(const TransformIterator<It>& other) const
			{
				return mIt != other.mIt;
			}

			bool operator==(const TransformIterator<It>& other) const
			{
				return mIt == other.mIt;
			}

		private:
			It mIt;
		};

	public:
		using iterator = TransformIterator<decltype(std::declval<T>().begin())>;
		using const_iterator = TransformIterator<decltype(std::declval<const T>().begin())>;

		iterator begin()
		{
                    return T::begin();
		}

		const_iterator begin() const
		{
                    return T::begin();
		}

		iterator end()
		{
                    return T::end();
		}

		const_iterator end() const
		{
			return T::end();
		}

	};

	template <typename Converter, class T>
	const TransformItContainer<T, Converter>& transformIt(const T& t)
	{
            return static_cast<const TransformItContainer<T, Converter> &>(t);
	}

	template <typename Converter, class T>
	TransformItContainer<T, Converter>& transformIt(T& t)
	{
            return static_cast<TransformItContainer<T, Converter> &>(t);
	}

	struct ToPointerConverter
	{
		template <class T>
		T* operator()(T& t)
		{
			return &t;
		}
	};

	template <class T>
	decltype(auto) toPointer(T &t)
	{
		return transformIt<ToPointerConverter>(t);
	}

	struct UniquePtrToPtrConverter
	{
		template <class T>
		T* operator()(const std::unique_ptr<T> &p)
		{
			return p.get();
		}
	};

	template <class T>
	decltype(auto) uniquePtrToPtr(const T &t)
	{
		return transformIt<UniquePtrToPtrConverter>(t);
	}

	
}