#include "FusionCore.h"

namespace CE
{

    FTimer::FTimer()
    {
        if (IsDefaultInstance())
            return;

        FusionApplication* app = FusionApplication::TryGet();
        if (app)
        {
            app->timers.Add(this);
        }
    }

    FTimer::~FTimer()
    {
        FusionApplication* app = FusionApplication::TryGet();
        if (app)
        {
            app->timers.Remove(this);
        }
    }

    void FTimer::Reset()
    {
        totalTimeTaken = 0;
    }

    void FTimer::Start(int milliseconds)
    {
        this->milliseconds = milliseconds;
        isActive = true;
    }

    void FTimer::Start()
    {
        isActive = true;
    }

    void FTimer::Restart()
    {
        totalTimeTaken = 0;
        isActive = true;
    }

    void FTimer::Stop()
    {
        isActive = false;
    }

    void FTimer::Tick()
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

            m_OnTimeOut();

			if (isSingleShot)
			{
				Stop();
				BeginDestroy();
			}
		}

		prevTime = clock();
    }

    
} // namespace CE
