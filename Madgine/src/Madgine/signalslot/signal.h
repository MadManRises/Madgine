#pragma once

#include "signalstub.h"


namespace Engine
{
	namespace SignalSlot
	{

		template <class... _Ty>
		class Signal : public SignalStub<_Ty...>
		{
		public:
			Signal() = default;

			Signal(const Signal<_Ty...>& other) = default;

			Signal(Signal<_Ty...>&& other) noexcept = default;

			~Signal()
			{
				this->disconnectAll();
			}
			
			void emit(_Ty ... args)
			{
				auto end = this->mConnectedSlots.end();

				this->mConnectedSlots.erase(
					std::remove_if(
						this->mConnectedSlots.begin(),
						end,
						[&](const std::weak_ptr<ConnectionInstance<_Ty...>> &p) {
							if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = p.lock())
							{
								(*ptr)(p, args...);
								return false;
							}
							else
							{
								return true;
							}
						}
					),
					end
				);

			}		
			
		};
	}
}
