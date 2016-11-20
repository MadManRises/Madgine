#pragma once

#include "ClassGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class EntityComponentGenerator : public ClassGenerator {
			public:
				EntityComponentGenerator(Module *module, const QString &name, const QString &componentName);
				EntityComponentGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:

				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

				void init();
			};

		}
	}
}