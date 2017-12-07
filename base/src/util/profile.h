#pragma once


namespace Engine
{
	namespace Util
	{

		class MADGINE_BASE_EXPORT ProfileWrapper
		{
		public:
			ProfileWrapper(const std::string& name);
			~ProfileWrapper();
		};

#define PROFILE(TARGET) Engine::Util::ProfileWrapper __p(TARGET)
	}
}
