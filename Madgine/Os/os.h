#pragma once

namespace Engine {

class Os {
public:

    //static std::list<std::string> filesMatchingPattern(const std::string &pattern);
    static void MADGINE_EXPORT createDir(const std::string &dirName);
	static void MADGINE_EXPORT remove(const std::string &fileName);

};

}



