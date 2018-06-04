#include "maditormodellib.h"

#include "guihandlergenerator.h"


namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString GuiHandlerGenerator::sType = "GuiHandler";

			GuiHandlerGenerator::GuiHandlerGenerator(Module * module, const QString & name, const QString &window, int type, bool hasLayoutFile) :
				ClassGenerator(module, name, sType)
			{
				element().setAttribute("window", window);
				element().setAttribute("windowType", type);
				element().setAttribute("layout", hasLayoutFile);

				init();

				generate();
			}
			GuiHandlerGenerator::GuiHandlerGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
				init();
			}
			QString GuiHandlerGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "GuiHandler.cpp";
				case 1:
					return "GuiHandler.h";
				default:
					throw 0;
				}
			}

			void GuiHandlerGenerator::init()
			{
				QString type;
				int typeIndex = intAttribute("windowType");
				switch (typeIndex) {
				case 0:
					type = "WindowType::MODAL_OVERLAY";
					break;
				case 1:
					type = "WindowType::NONMODAL_OVERLAY";
					break;
				case 2:
					type = "WindowType::ROOT_WINDOW";
					break;
				default:
					throw 0;
				}

				QString layout = (boolAttribute("layout") ? QString(", \"%1\"").arg(stringAttribute("window")) : "");

				setKeys({
					{"window", stringAttribute("window")},
					{"type", type},
					{"layout", layout}
				});
			}

		}
	}
}
