#pragma once

#include "Madgine/serialize/serializable.h"

namespace Maditor
{
	namespace Shared
	{
		struct MADITOR_SHARED_EXPORT ProjectFile : public Engine::Serialize::Serializable
		{
			ProjectFile();

			ProjectFile(const std::experimental::filesystem::path& fullPath, const std::experimental::filesystem::path &root, const std::experimental::filesystem::path& localRoot);

			bool compare(const std::experimental::filesystem::path &root) const;

			static std::experimental::filesystem::path make_relative(const std::experimental::filesystem::path& path,
			                                                         const std::experimental::filesystem::path& root);

			static uint64_t calculateSimpleHash(const std::experimental::filesystem::path& path);

			void readState(Engine::Serialize::SerializeInStream& in) override;

			void writeState(Engine::Serialize::SerializeOutStream& out) const override;

			std::experimental::filesystem::path mSourcePath;
			std::experimental::filesystem::path mPath;
			size_t mSize;
			uint64_t mSimpleHash;
		};
	}
}
