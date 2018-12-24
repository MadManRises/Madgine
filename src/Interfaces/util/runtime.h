#pragma once

namespace Engine {

	INTERFACES_EXPORT std::experimental::filesystem::path runtimePath();

	INTERFACES_EXPORT std::set<std::string> enumerateLoadedLibraries();

}