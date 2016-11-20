#pragma once

#include "ClassGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class GlobalAPIGenerator : public ClassGenerator {
			public:
				GlobalAPIGenerator(Module *module, const QString &name);
				GlobalAPIGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:



				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

			};

		}
	}
}