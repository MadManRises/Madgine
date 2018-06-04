#include "maditormodellib.h"

#include "cmakegenerator.h"
#include "cmakeproject.h"

namespace Maditor {
	namespace Model {
		namespace Generators {


			CmakeGenerator::CmakeGenerator(CmakeProject *parent, const QString & name) :
				Generator(false),
				mName(name),
				mParent(parent)
			{
				mParent->addSubProject(this);
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

			const QStringList &CmakeGenerator::files() const
			{
				return mFileList;
			}

			const QString & CmakeGenerator::name()
			{
				return mName;
			}

			QStringList CmakeGenerator::filePaths()
			{
				return{ mParent->root() + mName + "/" + "CmakeLists.txt" };
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

			void CmakeGenerator::addConfig(const QString & config)
			{
				mConfigs << config;
			}

			void CmakeGenerator::removeConfig(const QString & config)
			{
				mConfigs.removeAll(config);
			}

			const QStringList & CmakeGenerator::dependencies() const
			{
				return mDependencies;
			}

			const QStringList & CmakeGenerator::libraryDependencies() const
			{
				return mLibraryDependencies;
			}

			void CmakeGenerator::write(QTextStream & stream, int index)
			{
				QString files = mFileList.join(" ");
				
				QString dependencies = mDependencies.empty() ? "" : QString("${config}_") + mDependencies.join(" ${config}_");
				for (const QString &libraryDep : mLibraryDependencies)
					dependencies += QString(" ${${config}_%1_LIBRARIES}").arg(libraryDep);
				
				QString link = QString("target_link_libraries(${config}_%1 PUBLIC %2)").arg(mName, dependencies);

				stream << templateFile("Cmake.txt").arg(mName, files, link, mConfigs.join(" "));

			}

		}
	}
}
