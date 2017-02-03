#pragma once



namespace Engine {
	namespace Util {

		class MADGINE_BASE_EXPORT MadgineObjectCollector {

		public:
			void add(BaseMadgineObject *object);
			void remove(BaseMadgineObject *object);

			std::list<BaseMadgineObject*>::const_iterator begin();
			std::list<BaseMadgineObject*>::const_iterator end();

			static MadgineObjectCollector &getSingleton();

		private:
			std::list<BaseMadgineObject*> mObjects;
		};

	}
}