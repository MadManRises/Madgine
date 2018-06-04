#pragma once

#include "classgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			class GuiHandlerGenerator : public ClassGenerator {
			public:
				GuiHandlerGenerator(Module *module, const QString &name, const QString &window, int type, bool hasLayoutFile);
				GuiHandlerGenerator(Module *module, QDomElement data);

				static const QString sType;

			protected:

				// Inherited via ClassGenerator
				virtual QString templateFileName(int index) override;

				void init();
			};

		}
	}
}
