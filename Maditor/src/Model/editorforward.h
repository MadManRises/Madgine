#pragma once

#include "forward.h"


namespace Maditor {
	namespace Model {
		class ApplicationWrapper;
		class Project;

		namespace Watcher {
			class ApplicationWatcher;
		}

		namespace Generator {
			class ClassGeneratorFactory;
		}

		namespace Editors {
			class ScriptEditorModel;
			class VSLink;
		}
		
		class ModuleLoader;

	}
}