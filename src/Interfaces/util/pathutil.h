#pragma once

namespace Engine {
	namespace PathUtil {

		INTERFACES_EXPORT std::experimental::filesystem::path relative(const std::experimental::filesystem::path &p,
			const std::experimental::filesystem::path &base);

		INTERFACES_EXPORT std::experimental::filesystem::path make_normalized(const char *p);

	}
}