#pragma once

namespace Maditor {
	namespace Model {
		namespace Generators {
			class Generator {
			public:
				Generator(bool askOverride = true);

				virtual void generate();

				virtual QStringList filePaths() = 0;

			protected:
				virtual void write(QTextStream &stream, int index) = 0;

				QString templateFile(const QString &name);

			private:
				bool mAskOverride;
			};
		}
	}
}