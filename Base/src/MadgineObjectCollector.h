#pragma once



namespace Engine {

		class MADGINE_BASE_EXPORT MadgineObjectCollector {

		public:
			void add(MadgineObjectBase *object);
			void remove(MadgineObjectBase *object);

			std::list<MadgineObjectBase*>::const_iterator begin();
			std::list<MadgineObjectBase*>::const_iterator end();

			static MadgineObjectCollector &getSingleton();

		private:
			std::list<MadgineObjectBase*> mObjects;
		};

}