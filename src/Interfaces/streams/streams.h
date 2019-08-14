#pragma once

namespace Engine
{
	
	typedef std::istream::pos_type pos_type;

	struct INTERFACES_EXPORT InStream {
		InStream(std::unique_ptr<std::streambuf> &&buffer);
		InStream(InStream &&other);
		~InStream();

		template <typename T>
		InStream &operator>>(T &t)
		{
			mStream >> t;
			return *this;
		}

		std::istreambuf_iterator<char> begin();
		std::istreambuf_iterator<char> end();

		bool read(void *buffer, size_t size);

		operator bool() const;

	protected:
		InStream(std::streambuf *buffer);

		std::streambuf &buffer() const;

		pos_type tell();
		void seek(pos_type p);

	private:
		std::istream mStream;
		bool mOwning = true;
	};

	struct INTERFACES_EXPORT OutStream {
		OutStream(std::unique_ptr<std::streambuf> &&buffer);
		OutStream(OutStream &&other);
		~OutStream();

		template <typename T>
		OutStream &operator<<(const T &t)
		{
			mStream << t;
			return *this;
		}

		void write(const void *data, size_t count);

		operator bool() const;

	protected:
		std::streambuf &buffer() const;

	private:
		std::ostream mStream;
	};

	struct INTERFACES_EXPORT Stream : InStream, OutStream {
		Stream(std::unique_ptr<std::streambuf> &&buffer);

		using InStream::buffer;
	};

}