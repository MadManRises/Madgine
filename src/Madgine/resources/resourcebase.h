#pragma once

#include "Interfaces/filesystem/path.h"

namespace Engine
{
	namespace Resources
	{

		class MADGINE_BASE_EXPORT ResourceBase
		{

		public:
			ResourceBase(Filesystem::Path path);

			virtual ~ResourceBase() = default;

			virtual void setPersistent(bool b);
			virtual bool load() = 0;

			bool isPersistent() const;

			const Filesystem::Path &path();
			std::string extension();
			std::string name();

			void updatePath(const Filesystem::Path &path) { throw 0; }//TODO

		private:
			bool mIsPersistent;

			Filesystem::Path mPath;

		};

	}
}