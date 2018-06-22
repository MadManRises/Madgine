#pragma once

#include "generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class MADITOR_MODEL_EXPORT CmakeProject : public Generator {
			public:
				CmakeProject(const QDir &root, const QDir &buildDir, const QString &name);

				void build();

				// Geerbt über CmakeGenerator
				QDir root();
				void addSubProject(CmakeGenerator * sub);
				virtual void generate() override;
				virtual QStringList filePaths() override;
				const QStringList &libraries();

				void addLibrary(const QString &lib);

				QString solutionPath();

				QDir buildDir();

			protected:
				virtual void write(QTextStream &stream, int index) override;

			private:
				QString mName;
				QDir mBuildDir;
				QDir mRoot;

				QStringList mLibraries;


				std::vector<CmakeGenerator *> mSubProjects;

			};

		}
	}
}
