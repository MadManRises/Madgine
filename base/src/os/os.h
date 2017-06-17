#pragma once

namespace Engine {

class Os {
public:

    static std::list<std::string> MADGINE_BASE_EXPORT filesMatchingPattern(const std::string &path, const std::string &pattern);
    static void MADGINE_BASE_EXPORT createDir(const std::string &dirName);
	static void MADGINE_BASE_EXPORT remove(const std::string &fileName);

};

}



