#include "../interfaceslib.h"

#include "streams.h"

namespace Engine
{



	InStream::InStream(std::unique_ptr<std::streambuf>&& buffer) :
		mStream(buffer.release())
	{
	}

	InStream::~InStream()
	{
		if (mOwning)
			delete buffer();
	}

	std::istreambuf_iterator<char> InStream::begin()
	{
		return std::istreambuf_iterator<char>(mStream);
	}

	std::istreambuf_iterator<char> InStream::end()
	{
		return std::istreambuf_iterator<char>();
	}

	InStream::InStream(std::streambuf * buffer) :
		mStream(buffer),
		mOwning(false)
	{
	}

	std::streambuf * InStream::buffer() const
	{
		return mStream.rdbuf();
	}

	OutStream::OutStream(std::unique_ptr<std::streambuf>&& buffer) :
		mStream(buffer.release())
	{
	}

	OutStream::~OutStream()
	{
		delete buffer();
	}

	std::streambuf * OutStream::buffer() const
	{
		return mStream.rdbuf();
	}

	Stream::Stream(std::unique_ptr<std::streambuf>&& buffer) :
		InStream(buffer.get()),
		OutStream(std::move(buffer))
	{
	}

}