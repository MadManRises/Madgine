#include "ClassGenerator.h"
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
				return QString();
			}
		}
	}
}