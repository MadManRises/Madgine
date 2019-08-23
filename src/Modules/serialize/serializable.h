#pragma once


namespace Engine
{
	namespace Serialize
	{
		class MODULES_EXPORT Serializable
		{
		public:
			explicit Serializable();
			Serializable(const Serializable&);
			Serializable(Serializable&&) noexcept;
			~Serializable();

			Serializable& operator=(const Serializable&);

			void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map);
			void setDataSynced(bool b);
			void setActive(bool active);

		protected:
			bool isActive() const;

		private:
			bool mActive = false;
		};

		template <typename OffsetPtr>
                struct _Serializable : Serializable{
					bool isActive() const {
                                            return !OffsetPtr::parent(this) || Serializable::isActive();
					}
				};
	}
}
