#pragma once

#include "Interfaces/filesystem/path.h"

#include "Modules/keyvalue/scopebase.h"

namespace Engine
{
	namespace Resources
	{

		struct MODULES_EXPORT ResourceBase : ScopeBase
		{
			ResourceBase(Filesystem::Path path);

			~ResourceBase() = default;

			void setPersistent(bool b);
			
			bool isPersistent() const;

			const Filesystem::Path &path();
			std::string extension();
			std::string name();

			void updatePath(const Filesystem::Path &path) { throw 0; }//TODO

			std::string readAsText();

		private:
			bool mIsPersistent;

			Filesystem::Path mPath;

		};

	}
}