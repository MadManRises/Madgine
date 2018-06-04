#pragma once

#include "generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class HeaderGuardGenerator : public Generator {

			public:
				HeaderGuardGenerator(Module *module, const QString &name);

				// Geerbt über Generator
				virtual QStringList filePaths() override;
				virtual void write(QTextStream & stream, int index) override;

				const QString &name();

				QString fileName();
				QString guardName();
				static QString fileName(const QString &name);
				static QString guardName(const QString &name);

			private:
				Module *mModule;
				QString mName;

			};

		}
	}
}
