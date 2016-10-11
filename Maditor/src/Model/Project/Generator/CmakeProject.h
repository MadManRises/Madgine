#pragma once

#include "CmakeGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

			class CmakeProject : public CmakeGenerator {
			public:
				CmakeProject(const QString &root, const QString &buildDir, const QString &name);

				void build();

				// Geerbt über CmakeGenerator
				virtual QString root() override;

				void addLibrary(const QString &lib);

			protected:
				virtual QString preTargetCode() override;

			private:
				QString mBuildDir;
				QString mRoot;

				QStringList mLibraries;

			};

		}
	}
}