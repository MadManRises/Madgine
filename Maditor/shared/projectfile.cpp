#include "maditorsharedlib.h"

#include "projectfile.h"

#include "Madgine/serialize/streams/serializestream.h"

namespace Maditor
{
	namespace Shared
	{
		ProjectFile::ProjectFile():
			mSize(0),
			mSimpleHash(0)
		{
		}

		ProjectFile::ProjectFile(const std::experimental::filesystem::path& path,
		                         const std::experimental::filesystem::path& root,
								 const std::experimental::filesystem::path& localRoot):
			mSourcePath(make_relative(path, localRoot)),
			mPath(root/path.filename()),
			mSize(file_size(path)),
			mSimpleHash(calculateSimpleHash(path))
		{
		}

		bool ProjectFile::compare(const std::experimental::filesystem::path& root) const
		{
			std::experimental::filesystem::path path = root / mPath;
			return exists(path) && mSize == file_size(path) && mSimpleHash == calculateSimpleHash(path);
		}

		std::experimental::filesystem::path ProjectFile::make_relative(const std::experimental::filesystem::path& path,
		                                                               const std::experimental::filesystem::path& root)
		{
			auto it = path.begin();
			for (const auto& seg : root)
			{
				if (seg != ".")
				{
					if (seg != *it)
					{
						throw 0;
					}
					++it;
				}
			}
			std::experimental::filesystem::path result;
			for (; it != path.end(); ++it)
			{
				result /= *it;
			}
			return result;
		}

		uint64_t ProjectFile::calculateSimpleHash(const std::experimental::filesystem::path& path)
		{
			//TODO
			uint64_t hash = 6327;
			std::ifstream file(path, std::ios::in | std::ios::binary);

			char c;
			while (file.get(c))
			{
				hash = (hash << 5) + hash + c;
			}

			return hash;
		}

		void ProjectFile::readState(Engine::Serialize::SerializeInStream& in)
		{
			std::string path;
			std::string sourcePath;
			in >> path >> sourcePath >> mSize >> mSimpleHash;
			mPath = path;
			mSourcePath = sourcePath;
		}

		void ProjectFile::writeState(Engine::Serialize::SerializeOutStream& out) const
		{
			out << mPath.generic_string() << mSourcePath.generic_string() << mSize << mSimpleHash;
		}
	}
}