#pragma once

namespace Engine
{
	
	struct INTERFACES_EXPORT InStream {
		InStream(std::unique_ptr<std::streambuf> &&buffer);
		~InStream();

		template <typename T>
		InStream &operator>>(T &t)
		{
			mStream >> t;
			return *this;
		}

		std::istreambuf_iterator<char> begin();
		std::istreambuf_iterator<char> end();

	protected:
		InStream(std::streambuf *buffer);

		std::streambuf *buffer() const;

	private:
		std::istream mStream;
		bool mOwning = true;
	};

	struct INTERFACES_EXPORT OutStream {
		OutStream(std::unique_ptr<std::streambuf> &&buffer);
		~OutStream();

		template <typename T>
		OutStream &operator<<(const T &t)
		{
			mStream << t;
			return *this;
		}

	protected:
		std::streambuf *buffer() const;

	private:
		std::ostream mStream;
	};

	struct INTERFACES_EXPORT Stream : InStream, OutStream {
		Stream(std::unique_ptr<std::streambuf> &&buffer);

		using InStream::buffer;
	};

}