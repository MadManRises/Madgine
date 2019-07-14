#pragma once

#ifndef STATIC_BUILD

#include "Interfaces/filesystem/path.h"

namespace Engine
{
	namespace Plugins
	{
		class MODULES_EXPORT Plugin
		{
		public:
			Plugin(std::string name = {}, std::string project = {}, Filesystem::Path path = {});
			~Plugin();

			bool isLoaded() const;
			bool load();
			bool unload();

			const void *getSymbol(const std::string &name) const;

			Filesystem::Path fullPath() const;

			const std::string &project() const;

		private:
			void *mModule;

			std::string mProject;
			std::string mName;
			Filesystem::Path mPath;

		};
	}
}

#endif