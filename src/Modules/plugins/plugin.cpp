#include "../moduleslib.h"

#ifndef STATIC_BUILD

#include "plugin.h"

#include "../generic/templates.h"
#include "Interfaces/dl/dlapi.h"

namespace Engine {
namespace Plugins {

    Plugin::Plugin(std::string name, std::string project, Filesystem::Path path)
        : mModule(nullptr)
        , mPath(std::move(path))
        , mProject(std::move(project))
        , mName(std::move(name))
    {
        if (mPath.empty() && !mName.empty()) {
#if WINDOWS
            mPath = mName;
#elif UNIX
            mPath = "lib" + mName + ".so";
#endif
        }
    }

    Plugin::~Plugin()
    {
        unload();
    }

    bool Plugin::isLoaded() const
    {
        return mModule != nullptr;
    }

    bool Plugin::load()
    {
        if (isLoaded())
            return true;

        LOG("Loading Plugin \"" << mName << "\"...");

        std::string errorMsg;

        try {
            mModule = openDll(mPath.str());
        } catch (const std::exception &e) {
            errorMsg = e.what();
            mModule = nullptr;
        }

        if (!isLoaded()) {
            LOG_ERROR("Load of plugin \"" << mName << "\" failed with error: " << errorMsg);
        } else {
            LOG("Success");
        }

        return isLoaded();
    }

    bool Plugin::unload()
    {
        if (!isLoaded())
            return true;

		closeDll(mModule);

        mModule = nullptr;

        return true;
    }

    const void *Plugin::getSymbol(const std::string &name) const
    {
        std::string fullName = name + "_" + mName;
        return getDllSymbol(mModule, fullName);
    }

    Filesystem::Path Plugin::fullPath() const
    {
        if (!isLoaded())
            return {};

		std::string fullName = "binaryInfo_" + mName;
		return getDllFullPath(mModule, fullName);
    }

    const std::string &Plugin::project() const
    {
        return mProject;
    }
}
}

#endif