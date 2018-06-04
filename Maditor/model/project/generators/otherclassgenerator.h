#pragma once

#include "classgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class OtherClassGenerator : public ClassGenerator {
			public:
				OtherClassGenerator(Module *module, const QString &name, bool headerOnly);
				OtherClassGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:


				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

			};

		}
	}
}
