#pragma once

#include "../filesystem/path.h"

namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT Plugin
		{
		public:
			Plugin(std::string name = {}, Filesystem::Path path = {});
			~Plugin();

			bool isLoaded() const;
			bool load();
			bool unload();

			const void *getSymbol(const std::string &name) const;

			Filesystem::Path fullPath() const;

		private:
			void *mModule;

			std::string mName;
			Filesystem::Path mPath;

		};
	}
}
