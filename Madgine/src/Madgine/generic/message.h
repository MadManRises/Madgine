#pragma once

namespace Engine
{
	template <unsigned int N>
	class Message
	{
	public:
		constexpr Message(std::array<const char *, N>&& data) :
			mData(std::forward<std::array<const char *, N>>(data))
		{
		}

		template <typename... Ty, typename T = std::enable_if_t<sizeof...(Ty) + 1 == N>>
		std::string operator()(Ty&&... arg) const
		{
			std::stringstream ss;
			ss << mData[0];
			using expander = int[];
			unsigned int i = 0;

			(void)expander{(void(ss << std::forward<Ty>(arg) << mData[++i]), 0)...};

			return ss.str();
		}

	private:
		std::array<const char *, N> mData;
	};

	template <>
	class Message<1>
	{
	public:
		constexpr Message(const std::array<const char *, 1>& data) :
			mData(data[0])
		{
		}

		constexpr operator const char *() const
		{
			return mData;
		}

		operator std::string() const
		{
			return mData;
		}

	private:
		const char* mData;
	};

	template <>
	class Message<0>;

	template <typename... Ty>
	constexpr Message<sizeof...(Ty)> message(Ty&&... arg)
	{
		return {{{std::forward<Ty>(arg)...}}};
	}
}
