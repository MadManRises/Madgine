#include "madgineinclude.h"

#include "Model\Project\Module.h"
#include "HeaderGuardGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {
			HeaderGuardGenerator::HeaderGuardGenerator(Module * module, const QString & name) :
				mModule(module),
				mName(name)
			{
			}

			QStringList HeaderGuardGenerator::paths()
			{
				return{ mModule->root() + fileName() };
			}

			void HeaderGuardGenerator::write(QTextStream & stream, int index)
			{
				stream << templateFile("HeaderGuard.h").arg(mName, guardName());
			}

			const QString & HeaderGuardGenerator::name()
			{
				return mName;
			}

			QString HeaderGuardGenerator::fileName()
			{
				return fileName(mName);
			}

			QString HeaderGuardGenerator::guardName()
			{
				return guardName(mName);
			}

			QString HeaderGuardGenerator::fileName(const QString & name)
			{
				return name + "_module.h";
			}

			QString HeaderGuardGenerator::guardName(const QString & name)
			{
				return name.toUpper() + "_EXPORT";
			}


		}
	}
}