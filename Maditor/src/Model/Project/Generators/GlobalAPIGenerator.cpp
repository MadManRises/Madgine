#include "maditorlib.h"

#include "GlobalAPIGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString GlobalAPIGenerator::sType = "GlobalAPI";

			GlobalAPIGenerator::GlobalAPIGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			GlobalAPIGenerator::GlobalAPIGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString GlobalAPIGenerator::templateFileName(int index)
			{
				return QString();
			}
		}
	}
}