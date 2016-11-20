#pragma once

#include "ClassGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class SceneComponentGenerator : public ClassGenerator {
			public:
				SceneComponentGenerator(Module *module, const QString &name);
				SceneComponentGenerator(Module * module, QDomElement data);

				static const QString sType;

			protected:


				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

			};

		}
	}
}