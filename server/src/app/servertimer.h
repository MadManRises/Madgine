#pragma once


namespace Engine {
	namespace App {

		class ServerTimer
		{
		public:

			enum unit { MS, US, NS } default_unit;

			ServerTimer();

			ServerTimer(unit UN);

			void start(void);
			void reset(void);
			void endex(void);
			//------------------------------------------------------------------------------
			void delay(std::size_t duration);
			static void delay(std::size_t duration, ServerTimer::unit type);
			static std::size_t current_time(void);
			//------------------------------------------------------------------------------
			std::size_t elapsed_xx(ServerTimer::unit type);
			//------------------------------------------------------------------------------
			std::size_t elapsed_ms(void);
			std::size_t elapsed_us(void);
			std::size_t elapsed_ns(void);

		private:
			bool m_running;
			std::chrono::time_point<std::chrono::steady_clock> m_start;
			std::chrono::time_point<std::chrono::steady_clock> m_endex;
		};

	}
}