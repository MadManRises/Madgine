#include "maditorlib.h"

#include "CmakeGenerator.h"
#include "CmakeSubProject.h"

namespace Maditor {
	namespace Model {
		namespace Generators {


			CmakeGenerator::CmakeGenerator(const QString & name, const QString & targetName) :
				Generator(false),
				mName(name),
				mTargetName(targetName.isEmpty() ? name : targetName)
			{
			}

			void CmakeGenerator::addFile(const QString & file)
			{
				mFileList << file;
			}

			void CmakeGenerator::addFiles(const QStringList & files)
			{
				mFileList << files;
			}

			void CmakeGenerator::removeFiles(const QStringList & files)
			{
				for (const QString &file : files) {
					mFileList.removeAll(file);
				}
			}

			const QString & CmakeGenerator::name()
			{
				return mName;
			}

			QStringList CmakeGenerator::filePaths()
			{
				return{ root() + "CmakeLists.txt" };
			}

			void CmakeGenerator::addSubProject(CmakeSubProject * sub)
			{
				mSubProjects.push_back(sub);
			}

			void CmakeGenerator::generate()
			{
				Generator::generate();
				for (CmakeSubProject *sub : mSubProjects) {
					sub->generate();
				}
			}

			void CmakeGenerator::addDependency(const QString & dependency)
			{
				mDependencies << dependency;
			}

			void CmakeGenerator::removeDependency(const QString & dependency)
			{
				mDependencies.removeAll(dependency);
			}

			void CmakeGenerator::addLibraryDependency(const QString & dependency)
			{
				mLibraryDependencies << dependency;
			}

			const QStringList & CmakeGenerator::dependencies()
			{
				return mDependencies;
			}

			void CmakeGenerator::write(QTextStream & stream, int index)
			{
				QString files;
				for (const QString &f : mFileList)
					files += " " + f;

				QString subProjects;
				for (CmakeSubProject *sub : mSubProjects)
					subProjects += "add_subdirectory(" + sub->name() + ")\n";
				
				QString dependencies = mDependencies.join(" ");
				for (const QString &libraryDep : mLibraryDependencies)
					dependencies += QString(" ${%1_LIBRARIES}").arg(libraryDep);
				
				subProjects += QString("target_link_libraries(%1 %2)").arg(mTargetName, dependencies);

				stream << templateFile("Cmake.txt").arg(mName, mTargetName, files, subProjects, preTargetCode());

			}
			QString CmakeGenerator::preTargetCode()
			{
				return "";
			}
		}
	}
}