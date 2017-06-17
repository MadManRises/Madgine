#pragma once



namespace Engine {

		class MADGINE_BASE_EXPORT MadgineObjectCollector {

		public:
			void add(MadgineObject *object);
			void remove(MadgineObject *object);

			std::list<MadgineObject*>::const_iterator begin();
			std::list<MadgineObject*>::const_iterator end();

			static MadgineObjectCollector &getSingleton();

		private:
			std::list<MadgineObject*> mObjects;
		};

}