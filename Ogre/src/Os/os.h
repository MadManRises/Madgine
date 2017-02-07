#pragma once

namespace Engine {

class Os {
public:

    //static std::list<std::string> filesMatchingPattern(const std::string &pattern);
    static void OGREMADGINE_EXPORT createDir(const std::string &dirName);
	static void OGREMADGINE_EXPORT remove(const std::string &fileName);

};

}



