#pragma once

#include "generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class MADITOR_MODEL_EXPORT CmakeProject : public Generator {
			public:
				CmakeProject(const QString &root, const QString &buildDir, const QString &name);

				void build();

				// Geerbt über CmakeGenerator
				QString root();
				void addSubProject(CmakeGenerator * sub);
				virtual void generate() override;
				virtual QStringList filePaths() override;
				const QStringList &libraries();

				void addLibrary(const QString &lib);

				QString solutionPath();

				QString buildDir();

			protected:
				virtual void write(QTextStream &stream, int index) override;

			private:
				QString mName;
				QString mBuildDir;
				QString mRoot;

				QStringList mLibraries;


				std::list<CmakeGenerator *> mSubProjects;

			};

		}
	}
}
