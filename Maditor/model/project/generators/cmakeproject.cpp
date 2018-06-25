#include "maditormodellib.h"

#include "cmakeproject.h"

#include "commandline.h"

#include "cmakegenerator.h"

#include "Madgine/plugins/plugin.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			CmakeProject::CmakeProject(const QDir & root, const QDir &buildDir, const QString &name) :
				Generator(false),
				mBuildDir(buildDir),
				mRoot(root),
				mName(name)
			{
			}

			void CmakeProject::write(QTextStream & stream, int index)
			{
				if (index == 0) {
					QString libraries;
					for (const QString &lib : mLibraries) {
						libraries += QString(R"(find_package(%1 REQUIRED)
if (%1_FOUND)
	include_directories(${%1_INCLUDE_DIRS})
endif (%1_FOUND)
)").arg(lib);
					}


					QString subProjects;
					for (CmakeGenerator *sub : mSubProjects)
						subProjects += "add_subdirectory(" + sub->name() + ")\n";

					QDir dir(QApplication::applicationDirPath());
					dir.cdUp();
					stream << templateFile("CmakeMain.txt").arg(mName, libraries, subProjects, dir.path());
				}
			}


			void CmakeProject::addSubProject(CmakeGenerator * sub)
			{
				mSubProjects.push_back(sub);
			}

			void CmakeProject::removeSubProject(CmakeGenerator* sub)
			{
				mSubProjects.erase(std::remove(mSubProjects.begin(), mSubProjects.end(), sub), mSubProjects.end());
			}

			void CmakeProject::generate()
			{
				Generator::generate();
				for (CmakeGenerator *sub : mSubProjects) {
					sub->generate();
				}
			}

			QStringList CmakeProject::filePaths()
			{
				return{ 
					mRoot.filePath("CMakeLists.txt")
				};
			}

			const QStringList & CmakeProject::libraries()
			{
				return mLibraries;
			}


			QDir CmakeProject::root()
			{
				return mRoot;
			}

			void CmakeProject::addLibrary(const QString & lib)
			{
				mLibraries << lib;
			}

			QDir CmakeProject::buildDir()
			{
				return mBuildDir.filePath("debug/build");
			}

		}
	}
}
