#pragma once

#include "generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class MADITOR_MODEL_EXPORT CmakeProject : public Generator {
			public:
				CmakeProject(const QDir &root, const QDir &buildDir, const QString &name);

				// Geerbt über CmakeGenerator
				
				virtual void generate() override;
				virtual QStringList filePaths() override;


				void addSubProject(CmakeGenerator * sub);
				void removeSubProject(CmakeGenerator * sub);

				void addLibrary(const QString &lib);
				const QStringList &libraries();

				QDir root();
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
