#include "../interfaceslib.h"

#include "pathutil.h"

#if _WIN32
#	define NOMINMAX
#	include <Windows.h>
#endif

namespace Engine {
	namespace PathUtil {



		std::experimental::filesystem::path relative(const std::experimental::filesystem::path & p, const std::experimental::filesystem::path & base)
		{
			std::experimental::filesystem::path::iterator it1, it2;
			for (it1 = p.begin(), it2 = base.begin(); it1 != p.end() && it2 != base.end(); ++it1, ++it2)
			{
				if (*it1 != *it2)
					return {};
			}
			if (it2 != base.end())
				return {};
			if (it1 == p.end())
				return ".";
			std::experimental::filesystem::path result;
			while (it1 != p.end())
			{
				result /= *it1;
				++it1;
			}
			return result;
		}

		std::experimental::filesystem::path make_normalized(const char * p)
		{
#if _WIN32

			char buffer[512];
			auto result = GetLongPathNameA(p, buffer, sizeof(buffer));
			assert(result > 0 && result < sizeof(buffer));
			buffer[0] = toupper(buffer[0]);
			/*for (int i = 0; i < result; ++i)
				if (buffer[i] == '\\')
					buffer[i] = '/';*/
			return { buffer };
#else
			return { p };
#endif
		}

	}
}