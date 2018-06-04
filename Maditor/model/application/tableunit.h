#pragma once

#include "model/tablemodel.h"

#include "Madgine/serialize/serializableunit.h"

namespace Maditor {
	namespace Model {


		class TableUnitBase : public TableModel {
			Q_OBJECT

		public:
			
			TableUnitBase(int columnCount);

			
		protected:
			template <class C>
			void setContainer(C &container) {
				container.connectCallback([&container, this](const typename C::const_iterator &it, int op) {
					typename C::const_iterator begin = container.begin();
					int row = std::distance(begin, it);
					handleOperation(row, op);
				});
			}

			void handleOperation(int row, int op);
			
		private:
			bool mResetting;

		};

		template <class T>
		class TableUnit : public TableUnitBase, public Engine::Serialize::SerializableUnit<T> {
		public:
			TableUnit(int columnCount) :
				TableUnitBase(columnCount) {}
			
		};

	}
}
