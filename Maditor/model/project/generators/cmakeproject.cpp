#include "maditormodellib.h"

#include "cmakeproject.h"

#include "commandline.h"

#include "cmakegenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			CmakeProject::CmakeProject(const QString & root, const QString &buildDir, const QString &name) :
				Generator(false),
				mBuildDir(buildDir),
				mRoot(root),
				mName(name)
			{
			}
			void CmakeProject::build()
			{
				std::string cmd = QString("\"cmake -G \"" CMAKE_GENERATOR "\" -B\"%1\" -H\"%2\"\"").arg(buildDir(), mRoot).toStdString();
				CommandLine::exec(cmd.c_str());
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
					mRoot + "CmakeLists.txt"
				};
			}

			const QStringList & CmakeProject::libraries()
			{
				return mLibraries;
			}


			QString CmakeProject::root()
			{
				return mRoot;
			}

			void CmakeProject::addLibrary(const QString & lib)
			{
				mLibraries << lib;
			}

			QString CmakeProject::solutionPath()
			{
				return buildDir() + mName + ".sln";
			}

			QString CmakeProject::buildDir()
			{
				return mBuildDir +  "debug/build/";
			}

		}
	}
}
