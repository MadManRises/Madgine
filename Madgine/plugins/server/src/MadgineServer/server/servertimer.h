#pragma once


namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerTimer
		{
		public:

			enum unit { MS, US, NS } default_unit;

			ServerTimer();

			ServerTimer(unit UN);

			void start();
			void reset();
			void endex();
			//------------------------------------------------------------------------------
			void delay(size_t duration);
			static void delay(size_t duration, unit type);
			static size_t current_time();
			//------------------------------------------------------------------------------
			size_t elapsed_xx(unit type);
			//------------------------------------------------------------------------------
			size_t elapsed_ms();
			size_t elapsed_us();
			size_t elapsed_ns();

		private:
			bool m_running;
			std::chrono::time_point<std::chrono::steady_clock> m_start;
			std::chrono::time_point<std::chrono::steady_clock> m_endex;
		};
	}
}
