#pragma once 



namespace Maditor {

	namespace View {
		namespace Dialogs {


			class SettingsTab : public QWidget {
			public:
				virtual void setup() = 0;
				virtual bool apply() = 0;
			};


		}
	}
}
