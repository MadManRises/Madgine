

#include "baselib.h"
#include "os.h"
#include <windows.h>
#undef NO_ERROR

namespace Engine {

std::list<std::string> Os::filesMatchingPattern(const std::string &path, const std::string &pattern)
{

    std::list<std::string> result;

    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind;


    hFind = FindFirstFileA((path + pattern).c_str(), &FindFileData);
    while (hFind != INVALID_HANDLE_VALUE) {
        result.emplace_back(path + FindFileData.cFileName);

        if (!FindNextFileA(hFind, &FindFileData)) {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    return result;

}

void Os::createDir(const std::string &dirName){
    CreateDirectoryA(dirName.c_str(), NULL);
}

void Os::remove(const std::string &fileName){
    ::remove(fileName.c_str());
}



}

