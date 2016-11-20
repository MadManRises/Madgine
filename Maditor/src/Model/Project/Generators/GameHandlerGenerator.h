#pragma once

#include "classGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class GameHandlerGenerator : public ClassGenerator {
			public:
				GameHandlerGenerator(Module *module, const QString &name);
				GameHandlerGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:


				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

			};

		}
	}
}