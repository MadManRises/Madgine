#pragma once

#include "generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class CmakeGenerator : public Generator {
			public:
				CmakeGenerator(CmakeProject *parent, const QString &name);

				void addFile(const QString &file);
				void addFiles(const QStringList &files);
				void removeFiles(const QStringList &files);

				const QStringList &files() const;

				const QString &name();

				virtual QStringList filePaths() override;



				void addDependency(const QString &dependency);
				void removeDependency(const QString &dependency);
				void addLibraryDependency(const QString &dependency);
				void addConfig(const QString &config);
				void removeConfig(const QString &config);

				const QStringList &dependencies() const;

				const QStringList & libraryDependencies() const;

			protected:
				virtual void write(QTextStream &stream, int index) override final;

			private:
				CmakeProject *mParent;

				QString mName;

				QStringList mFileList;

				QStringList mDependencies;

				QStringList mLibraryDependencies;

				QStringList mConfigs;

			};

		}
	}
}
