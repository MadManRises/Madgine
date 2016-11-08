#include "madgineinclude.h"

#include "SceneComponentGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

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
					return "SceneComponent.h";
				case 1:
					return "SceneComponent.cpp";
				default:
					throw 0;
				}
			}
		}
	}
}