#include "serverlib.h"
#include "servertimer.h"

namespace Engine
{
	namespace App
	{
		ServerTimer::ServerTimer() :
			default_unit(MS),
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

		void ServerTimer::start()
		{
			m_running = true;
			m_start = std::chrono::steady_clock::now();
		}

		void ServerTimer::reset()
		{
			this->start();
		}

		void ServerTimer::endex()
		{
			m_running = false;
			m_endex = std::chrono::steady_clock::now();
		}

		//------------------------------------------------------------------------------

		void ServerTimer::delay(size_t duration)
		{
			switch (default_unit)
			{
			case MS:
				std::this_thread::sleep_for(std::chrono::milliseconds(duration));
				break;
			case US:
				std::this_thread::sleep_for(std::chrono::microseconds(duration));
				break;
			case NS:
				std::this_thread::sleep_for(std::chrono::nanoseconds(duration));
			}
		}

		void ServerTimer::delay(size_t duration, unit type)
		{
			switch (type)
			{
			case MS:
				std::this_thread::sleep_for(std::chrono::milliseconds(duration));
				break;
			case US:
				std::this_thread::sleep_for(std::chrono::microseconds(duration));
				break;
			case NS:
				std::this_thread::sleep_for(std::chrono::nanoseconds(duration));
			}
		}

		size_t ServerTimer::current_time()
		{
			return std::chrono::steady_clock::now()
				.time_since_epoch().count();
		}

		//------------------------------------------------------------------------------

		size_t ServerTimer::elapsed_xx(unit type)
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			switch (type)
			{
			case MS:
				return std::chrono::duration_cast<std::chrono::milliseconds>
					(m_endex - m_start).count();
			case US:
				return std::chrono::duration_cast<std::chrono::microseconds>
					(m_endex - m_start).count();
			case NS:
				return std::chrono::duration_cast<std::chrono::nanoseconds>
					(m_endex - m_start).count();
			default:
				throw 0;
			}
		}

		//------------------------------------------------------------------------------

		size_t ServerTimer::elapsed_ms()
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			return std::chrono::duration_cast<std::chrono::milliseconds>
				(m_endex - m_start).count();
		}

		size_t ServerTimer::elapsed_us()
		{
			if (m_running)
			{
				m_endex = std::chrono::steady_clock::now();
			}
			return std::chrono::duration_cast<std::chrono::microseconds>
				(m_endex - m_start).count();
		}

		size_t ServerTimer::elapsed_ns()
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
