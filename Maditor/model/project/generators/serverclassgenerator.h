#pragma once

#include "classgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class MADITOR_MODEL_EXPORT ServerClassGenerator : public ClassGenerator {
			public:
				ServerClassGenerator(Module *module, const QString &name);
				ServerClassGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:


				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

			};

		}
	}
}
