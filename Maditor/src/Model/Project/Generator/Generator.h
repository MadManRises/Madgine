#pragma once

namespace Maditor {
	namespace Model {
		namespace Generator {
			class Generator {
			public:
				Generator(bool askOverride = true);

				virtual void generate();

				virtual QStringList paths() = 0;

			protected:
				virtual void write(QTextStream &stream, int index) = 0;

				QString templateFile(const QString &name);

			private:
				bool mAskOverride;
			};
		}
	}
}