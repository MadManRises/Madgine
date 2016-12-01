#include "maditorlib.h"

#include "CmakeProject.h"

#include "CommandLine.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			CmakeProject::CmakeProject(const QString & root, const QString &buildDir, const QString &name) :
				CmakeGenerator(name, "init"),
				mBuildDir(buildDir),
				mRoot(root)
			{
			}
			void CmakeProject::build(const QString &buildType)
			{
				std::string cmd = QString("\"\"" CMAKE_PATH "\" -G \"" CMAKE_GENERATOR "\" -B\"%1\" -H\"%2\"\" -DMADGINE_BUILD_TYPE=\"%3\"").arg(mBuildDir + buildType + "/build/", mRoot, buildType).toStdString();
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

				return templateFile("CmakeMain.txt").arg(name(), libraries, subProjects().join(' '));
			}

			QString CmakeProject::root()
			{
				return mRoot;
			}

			void CmakeProject::addLibrary(const QString & lib)
			{
				mLibraries << lib;
			}

			QString CmakeProject::solutionName()
			{
				return mBuildDir + name() + ".sln";
			}

		}
	}
}