#include "../interfaceslib.h"

#include "pathutil.h"
#include "stringutil.h"

#if _WIN32
#	define NOMINMAX
#	include <Windows.h>
#endif

namespace Engine {
	namespace PathUtil {


		std::experimental::filesystem::path relative(const std::experimental::filesystem::path & p, const std::experimental::filesystem::path & base)
		{
			std::experimental::filesystem::path::const_iterator it1 = p.begin(), it2 = base.begin();
			size_t baseCount = std::distance(it2, base.end());
			size_t count = std::distance(it1, p.end());

			if (baseCount >= count)
				return {};

			std::experimental::filesystem::path cmp;
			for (; it2 != base.end(); ++it1, ++it2)
				cmp /= *it1;
				
			if (!equivalent(base, cmp))
				return {};

			std::experimental::filesystem::path result;
			for (; it1 != p.end(); ++it1)
				result /= *it1;
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