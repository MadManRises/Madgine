#pragma once

namespace Engine
{
	class CopyType
	{
	};

	class NonCopyType
	{
	protected:
		NonCopyType() = default;
		NonCopyType(const NonCopyType&) = delete;
		void operator=(const NonCopyType&) = delete;
		NonCopyType(NonCopyType&&) = default;
		NonCopyType& operator=(NonCopyType&&) = default;
	};

	namespace __generic__impl__ {
		template <class, class T>
		struct _custom_is_copy_constructible : std::false_type
		{
		};

		template <class T>
		struct _custom_is_copy_constructible<std::void_t<decltype(T(std::declval<const T &>()))>, T> : std::true_type
		{
		};
	}

	template <class T>
	struct custom_is_copy_constructible
	{
		static const constexpr bool value = __generic__impl__::_custom_is_copy_constructible<std::void_t<>, T>::value;
	};

	template <class U, class V>
	struct custom_is_copy_constructible<std::pair<U, V>>
	{
		static constexpr const bool value = custom_is_copy_constructible<U>::value && custom_is_copy_constructible<V>::value;
	};

	template <class T>
	using CopyTraits = std::conditional_t<custom_is_copy_constructible<T>::value, CopyType, NonCopyType>;

	template <typename T, typename = std::enable_if_t<custom_is_copy_constructible<T>::value>>
	T tryCopy(T &t)
	{
		return t;
	}

	template <typename T, typename = std::enable_if_t<!custom_is_copy_constructible<std::remove_reference_t<T>>::value>>
	std::remove_reference_t<T> tryCopy(T &&t)
	{
		throw 0;
	}

}