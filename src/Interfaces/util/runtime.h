#pragma once

namespace Engine {

	INTERFACES_EXPORT std::experimental::filesystem::path runtimePath();

#ifndef STATIC_BUILD
	INTERFACES_EXPORT std::set<std::string> enumerateLoadedLibraries();
#endif

}