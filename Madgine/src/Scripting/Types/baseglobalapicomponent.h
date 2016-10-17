#pragma once


namespace Engine {
	namespace Scripting {

		class MADGINE_EXPORT BaseGlobalAPIComponent {

		public:
			virtual ~BaseGlobalAPIComponent() = default;

			virtual void init();
		};

	}
}