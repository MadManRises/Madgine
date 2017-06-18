#include "serverlib.h"
#include "servertimer.h"

namespace Engine {
	namespace App {

		ServerTimer::ServerTimer() :
			default_unit(unit::MS),
			m_running(true),
			m_start(std::chrono::steady_clock::now())
		{
			m_endex = m_start;
		}

		ServerTimer::ServerTimer(unit UN) :
			default_unit(UN),
			m_running(true),
			m_start(std::chrono::steady_clock::now())
		{
			m_endex = m_start;
		}

		void ServerTimer::start(void)
		{
			m_running = true;
			m_start = std::chrono::steady_clock::now();
		}

		void ServerTimer::reset(void)
		{
			this->start();
		}

		void ServerTimer::endex(void)
		{
			m_running = false;
			m_endex = std::chrono::steady_clock::now();
		}

		//------------------------------------------------------------------------------

		void ServerTimer::delay(std::size_t duration)
		{
			switch (default_unit)
			{
			case unit::MS:
				std::this_thread::sleep_for(std::chrono::milliseconds(duration));
				break;
			case unit::US:
				std::this_thread::sleep_for(std::chrono::microseconds(duration));
				break;
			case unit::NS:
				std::this_thread::sleep_for(std::chrono::nanoseconds(duration));
			}
		}

		void ServerTimer::delay(std::size_t duration, ServerTimer::unit type)
		{
			switch (type)
			{
			case unit::MS:
				std::this_thread::sleep_for(std::chrono::milliseconds(duration));
				break;
			case unit::US:
				std::this_thread::sleep_for(std::chrono::microseconds(duration));
				break;
			case unit::NS:
				std::this_thread::sleep_for(std::chrono::nanoseconds(duration));
			}
		}

		std::size_t ServerTimer::current_time(void)
		{
			return std::chrono::steady_clock::now()
				.time_since_epoch().count();
		}

		//------------------------------------------------------------------------------

		std::size_t ServerTimer::elapsed_xx(ServerTimer::unit type)
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			switch (type)
			{
			case unit::MS:
				return std::chrono::duration_cast<std::chrono::milliseconds>
					(m_endex - m_start).count();
				break;
			case unit::US:
				return std::chrono::duration_cast<std::chrono::microseconds>
					(m_endex - m_start).count();
				break;
			case unit::NS:
				return std::chrono::duration_cast<std::chrono::nanoseconds>
					(m_endex - m_start).count();
			}
		}

		//------------------------------------------------------------------------------

		std::size_t ServerTimer::elapsed_ms(void)
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			return std::chrono::duration_cast<std::chrono::milliseconds>
				(m_endex - m_start).count();
		}

		std::size_t ServerTimer::elapsed_us(void)
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			return std::chrono::duration_cast<std::chrono::microseconds>
				(m_endex - m_start).count();
		}

		std::size_t ServerTimer::elapsed_ns(void)
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			return std::chrono::duration_cast<std::chrono::nanoseconds>
				(m_endex - m_start).count();
		}

	}
}