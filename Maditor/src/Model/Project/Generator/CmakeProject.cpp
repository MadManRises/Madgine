#include "CmakeProject.h"

#include <memory>

#include "CommandLine.h"

namespace Maditor {
	namespace Model {
		namespace Generator {
			CmakeProject::CmakeProject(const QString & root, const QString &buildDir, const QString &name) :
				CmakeGenerator(name, "init"),
				mBuildDir(buildDir),
				mRoot(root)
			{
			}
			void CmakeProject::build()
			{
				std::string cmd = QString("\"\"" CMAKE_PATH "\" -G \"" CMAKE_GENERATOR "\" -B\"%1\" -H\"%2\"\"").arg(mBuildDir, mRoot).toStdString();
				CommandLine::exec(cmd.c_str());
			}

			QString CmakeProject::preTargetCode()
			{
				QString libraries;
				for (const QString &lib : mLibraries) {
					libraries += QString(R"(find_package(%1)
if (%1_FOUND)
	include_directories(${%1_INCLUDE_DIRS})
endif (%1_FOUND)
)").arg(lib);
				}

				return templateFile("CmakeMain.txt").arg(name(), libraries);
			}

			QString CmakeProject::root()
			{
				return mRoot;
			}

			void CmakeProject::addLibrary(const QString & lib)
			{
				mLibraries << lib;
			}

		}
	}
}