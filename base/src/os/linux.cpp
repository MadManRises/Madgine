#include "baselib.h"

#ifdef __linux__

#include "os.h"
#include <glob.h>
#include <sys/stat.h>
#include <stdio.h>

namespace Engine {

std::list<std::string> Os::filesMatchingPattern(const std::string &path, const std::string &pattern)
{
    std::list<std::string> result;

	glob_t g;

	if (glob((path + pattern).c_str(), 0, NULL, &g) == 0) {
		for (int i = 0; i < g.gl_pathc; ++i) {
			result.emplace_back(g.pathv[i]);
		}
	}

	globfree(&g);

	return result;
}

void Os::createDir(const std::string &dirName){
	mkdir(dirName.c_str, NULL);
}

void Os::remove(const std::string &fileName){
	::remove(fileName);
}



}

#endif
