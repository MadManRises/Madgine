#pragma once


namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT Plugin
		{
		public:
			Plugin(const std::experimental::filesystem::path &path);

			bool isLoaded();
			bool load();
			bool unload();

			void *getSymbol(const std::string &name) const;

			std::experimental::filesystem::path fullPath();

		private:
			void *mModule;

			std::experimental::filesystem::path mPath;

		};
	}
}