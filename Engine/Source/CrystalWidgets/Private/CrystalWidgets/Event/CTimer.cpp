#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CTimer::CTimer()
	{
		CApplication* app = CApplication::TryGet();
		if (app)
		{
			app->timers.Add(this);
		}
	}

	CTimer::~CTimer()
	{
		CApplication* app = CApplication::TryGet();
		if (app)
		{
			app->timers.Remove(this);
		}
	}

	void CTimer::Reset()
	{
		totalTimeTaken = 0;
	}

	void CTimer::Start(int milliseconds)
	{
		this->milliseconds = milliseconds;
		isActive = true;
	}

	void CTimer::Start()
	{
		isActive = true;
	}

	void CTimer::Restart()
	{
		totalTimeTaken = 0;
		isActive = true;
	}

	void CTimer::Stop()
	{
		isActive = false;
	}

	void CTimer::Tick()
	{
		if (!isActive)
		{
			return;
		}

		f32 timeTaken = (f32)(clock() - prevTime) / CLOCKS_PER_SEC;
		if (prevTime == 0)
			timeTaken = 0;

		totalTimeTaken += timeTaken;

		if (isActive && (int)(totalTimeTaken * 1000) > milliseconds)
		{
			totalTimeTaken = 0;

			OnTimeOut();

			if (isSingleShot)
			{
				Stop();
				Destroy();
			}
		}

		prevTime = clock();
	}

} // namespace CE::Widgets
