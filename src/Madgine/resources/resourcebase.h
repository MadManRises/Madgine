#pragma once

namespace Engine
{
	namespace Resources
	{

		class MADGINE_BASE_EXPORT ResourceBase
		{

		public:
			ResourceBase(const std::experimental::filesystem::path &path);

			virtual ~ResourceBase() = default;

			virtual void setPersistent(bool b);
			virtual bool load() = 0;

			bool isPersistent() const;

			const std::experimental::filesystem::path &path();
			std::string extension();
			std::string name();

			void updatePath(const std::experimental::filesystem::path &path) { throw 0; }//TODO

		private:
			bool mIsPersistent;

			std::experimental::filesystem::path mPath;

		};

	}
}