#pragma once


namespace Engine
{
	namespace Serialize
	{
		struct MODULES_EXPORT SerializableBase
		{		
                    SerializableBase();
                    SerializableBase(const SerializableBase &);
                    SerializableBase(SerializableBase &&) noexcept;
                    ~SerializableBase();

			SerializableBase &operator=(const SerializableBase &);

			void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map);
			void setDataSynced(bool b);
			void setActive(bool active);

		protected:
			bool isActive() const;

		private:
			bool mActive = false;
		};

		template <typename OffsetPtr>
                struct Serializable : SerializableBase {
					bool isActive() const {
                                            return !OffsetPtr::parent(this) || SerializableBase::isActive();
					}
                                        bool isSynced() const
                                        {
                                            return OffsetPtr::parent(this) && OffsetPtr::parent(this)->isSynced();
                                        }
				};
	}
}
