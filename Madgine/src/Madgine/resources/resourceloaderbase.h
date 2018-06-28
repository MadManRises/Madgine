#pragma once 

namespace Engine
{
	namespace Resources
	{
	
		class MADGINE_BASE_EXPORT ResourceLoaderBase
		{
		public:
			ResourceLoaderBase(ResourceManager &mgr, std::vector<std::string> &&extensions, bool autoLoad=false);

			

			virtual void addResource(const std::experimental::filesystem::path &path) = 0;

			void resourceAdded(ResourceBase* res);
			
			const std::vector<std::string> &fileExtensions() const;

		private:
			ResourceManager &mMgr;

			std::vector<std::string> mExtensions;

			bool mAutoLoad;

		};

	}
}