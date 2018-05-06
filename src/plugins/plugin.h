#pragma once


namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT Plugin
		{
		public:
			Plugin(const std::string &path);

			bool isLoaded();
			bool load();
			bool unload();

			void *getSymbol(const std::string &name);

		private:
			void *mModule;

			std::string mPath;

		};
	}
}
