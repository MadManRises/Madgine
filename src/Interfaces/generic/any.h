#pragma once

namespace Engine
{

	struct AnyHolderBase
	{
		virtual ~AnyHolderBase() = default;
		virtual std::unique_ptr<AnyHolderBase> copy() = 0;
	};

	template <typename T>
	struct AnyHolder : AnyHolderBase
	{
		AnyHolder(T &&init) :
			data(std::move(init))
		{
		}

		AnyHolder(const T &init) :
			data(init)
		{
		}

		std::unique_ptr<AnyHolderBase> copy() override
		{
			return std::make_unique<AnyHolder<T>>(data);
		}

		T data;
	};

	struct Any
	{
		template <typename T, typename = std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, Any>>>
		Any(T &&data) :
			mData(std::make_unique<AnyHolder<std::remove_reference_t<T>>>(std::forward<T>(data)))
		{
		}

		Any(Any &&other) :
			mData(std::move(other.mData))
		{}

		Any(const Any &other) :
			mData(other.mData ? other.mData->copy() : std::unique_ptr<AnyHolderBase>{})
		{}

		Any &operator=(Any &&other)
		{
			mData = std::move(other.mData);
			return *this;
		}

		Any &operator=(const Any &other)
		{
			mData = other.mData ? other.mData->copy() : std::unique_ptr<AnyHolderBase>{};
			return *this;
		}

		template <typename T>
		T &as() const
		{
			AnyHolder<T> *holder = dynamic_cast<AnyHolder<T>*>(mData.get());
			if (!holder)
				throw 0;
			return holder->data;
		}

	private:
		std::unique_ptr<AnyHolderBase> mData;
	};

}