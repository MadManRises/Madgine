#pragma once

#include "../toolscollector.h"

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ProjectManager : Tool<ProjectManager> {
        ProjectManager(ImRoot &root);

        virtual const char *key() const override;

        virtual void renderMenu() override;

		void setProjectRoot(const Filesystem::Path &root);

		const Filesystem::Path projectRoot() const;

		//TODO remove
		std::string mProjectRootString;

    private:
        Filesystem::Path mCurrentSelectionPath;
        Filesystem::Path mCurrentPath;

		Filesystem::Path mProjectRoot;
    };

}
}