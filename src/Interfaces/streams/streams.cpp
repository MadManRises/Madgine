#include "../interfaceslib.h"

#include "streams.h"

namespace Engine
{



	InStream::InStream(std::unique_ptr<std::streambuf>&& buffer) :
		mStream(buffer.release())
	{
	}

	InStream::InStream(InStream && other) :
		mStream(other.mStream.rdbuf(nullptr)),
		mOwning(std::exchange(other.mOwning, false))
	{
	}

	InStream::~InStream()
	{
		if (mOwning)
			delete &buffer();
	}

	std::istreambuf_iterator<char> InStream::begin()
	{
		return std::istreambuf_iterator<char>(mStream);
	}

	std::istreambuf_iterator<char> InStream::end()
	{
		return std::istreambuf_iterator<char>();
	}

	bool InStream::read(void * buffer, size_t size)
	{
		return mStream.read(static_cast<char*>(buffer), size).good();
	}

	pos_type InStream::tell()
	{
		return mStream.tellg();
	}

	void InStream::seek(pos_type p)
	{
		mStream.seekg(p);
	}


	InStream::operator bool() const
	{
		return static_cast<bool>(mStream);
	}

	InStream::InStream(std::streambuf * buffer) :
		mStream(buffer),
		mOwning(false)
	{
	}

	std::streambuf &InStream::buffer() const
	{
		return *mStream.rdbuf();
	}

	OutStream::OutStream(std::unique_ptr<std::streambuf>&& buffer) :
		mStream(buffer.release())
	{
	}

	OutStream::OutStream(OutStream && other) :
		mStream(other.mStream.rdbuf(nullptr))
	{
	}

	OutStream::~OutStream()
	{
		delete &buffer();
	}

	void OutStream::write(const void * data, size_t count)
	{
		mStream.write(static_cast<const char*>(data), count);
	}

	std::streambuf &OutStream::buffer() const
	{
		return *mStream.rdbuf();
	}

	OutStream::operator bool() const
	{
		return static_cast<bool>(mStream);
	}

	Stream::Stream(std::unique_ptr<std::streambuf>&& buffer) :
		InStream(buffer.get()),
		OutStream(std::move(buffer))
	{
	}

}