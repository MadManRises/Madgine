#pragma once

#include "Generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class CmakeGenerator : public Generator {
			public:
				CmakeGenerator(const QString &name, const QString &targetName = "");

				void addFile(const QString &file);
				void addFiles(const QStringList &files);
				void removeFiles(const QStringList &files);

				virtual QString root() = 0;
				const QString &name();

				virtual QStringList filePaths() override;

				void addSubProject(CmakeSubProject *sub);

				virtual void generate() override;

				void addDependency(const QString &dependency);
				void removeDependency(const QString &dependency);
				void addLibraryDependency(const QString &dependency);

				const QStringList &dependencies();

				QStringList subProjects();

			protected:
				virtual void write(QTextStream &stream, int index) override final;

				virtual QString preTargetCode();

			private:
				QString mName, mTargetName;

				QStringList mFileList;

				std::list<CmakeSubProject *> mSubProjects;
				QStringList mDependencies;
				QStringList mLibraryDependencies;

			};

		}
	}
}