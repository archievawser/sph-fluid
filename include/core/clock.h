#pragma once
#include <chrono>
#include <string>
#include "utils/utils.h"


namespace Core
{
	template<class Resolution = std::chrono::milliseconds>
	class Clock
	{
	public:
		Clock() 
			: mStart(ClockType::now())
		{ }

	public:
		Resolution::rep Restart()
		{
			const auto elapsed = GetElapsed();
			mStart = ClockType::now();
			return elapsed;
		}

		Resolution::rep GetElapsed() const
		{
			return std::chrono::duration_cast<Resolution>(ClockType::now() - mStart).count();
		}

	public:
		using ClockType = std::chrono::high_resolution_clock;

	private:
		ClockType::time_point mStart;
	};

	template<class Resolution = std::chrono::milliseconds>
	class ScopeTimer : public Clock<Resolution>
	{
	public:
		ScopeTimer(std::string name)
			: Base(), mName(name)
		{ }
		
		~ScopeTimer()
		{ 
			auto elapsed = this->GetElapsed();
			RABID_LOG(mName << " existed for " << elapsed);
		}

	public:
		typedef Clock<Resolution> Base;

	private:
		std::string mName;
	};
}