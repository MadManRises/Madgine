#pragma once

#include "../serializable.h"

namespace Engine
{
	namespace Serialize
	{
		class MODULES_EXPORT FileBuffer : std::vector<char>, public SerializableBase
		{
		public:
			using vector::vector;

			friend class FileBufferWriter;
			friend class FileBufferReader;

			void writeState(SerializeOutStream& out) const;
			void readState(SerializeInStream& in);

			const_iterator begin() const;
			const_iterator end() const;

			static FileBuffer readFile(const Filesystem::Path &path);
			void writeFile(const Filesystem::Path &path, bool createFolders = false) const;
		};

		class MODULES_EXPORT FileBufferReader : public std::basic_streambuf<char>
		{
		public:
			FileBufferReader(FileBuffer& buffer);

			pos_type seekoff(off_type off, std::ios_base::seekdir dir,
			                 std::ios_base::openmode mode = std::ios_base::in) override;
			pos_type seekpos(pos_type pos,
			                 std::ios_base::openmode mode = std::ios_base::in) override;

		private:
			FileBuffer& mBuffer;
		};

		class MODULES_EXPORT FileBufferWriter : public std::basic_streambuf<char>
		{
		public:
			FileBufferWriter(FileBuffer& buffer);
			~FileBufferWriter();

		protected:
			void extend();

			int_type overflow(int c = EOF) override;
			int sync() override;

		private:
			static constexpr size_t BUFFER_SIZE = 100;

			std::list<std::array<char, BUFFER_SIZE>> mDataBuffer;

			size_t bufferByteSize() const;

			FileBuffer& mBuffer;
		};
	} // namespace Serialize
} // namespace Core
