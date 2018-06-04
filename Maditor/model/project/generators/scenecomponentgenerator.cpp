#include "maditormodellib.h"

#include "scenecomponentgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString SceneComponentGenerator::sType = "SceneComponent";

			SceneComponentGenerator::SceneComponentGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			SceneComponentGenerator::SceneComponentGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString SceneComponentGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "SceneComponent.cpp";
				case 1:
					return "SceneComponent.h";					
				default:
					throw 0;
				}
			}
		}
	}
}
