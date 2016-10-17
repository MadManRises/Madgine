#pragma once

#include "serializable.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

class MADGINE_EXPORT FileBuffer : std::vector<char>, public Serialize::Serializable{
public:
    FileBuffer();

	friend class FileBufferWriter;
	friend class FileBufferReader;

protected:
    virtual void save(SerializeOutStream &out) const override;
    virtual void load(SerializeInStream &in) override;
};

class MADGINE_EXPORT FileBufferReader : public std::basic_streambuf<char> {
public:
	FileBufferReader(FileBuffer &buffer);


private:
	FileBuffer &mBuffer;
};

class MADGINE_EXPORT FileBufferWriter : public std::basic_streambuf<char> {
public:
    FileBufferWriter(FileBuffer &buffer);
    ~FileBufferWriter();

protected:
    void extend();

    virtual int_type overflow(int c = EOF) override;
    virtual int sync() override;

private:
    static constexpr size_t BUFFER_SIZE = 100;

    std::list<std::array<char, BUFFER_SIZE>> mDataBuffer;

    size_t bufferByteSize() const;

    FileBuffer &mBuffer;
};




} // namespace Serialize
} // namespace Scripting
} // namespace Core

