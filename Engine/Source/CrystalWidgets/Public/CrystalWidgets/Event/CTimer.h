#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CTimer final : public Object
    {
        CE_CLASS(CTimer, Object)
    public:

        CTimer();
        virtual ~CTimer();

        void Reset();

        void Start(int milliseconds);

    	void Start();

        void Restart();

        void Stop();

        b8 IsSingleShot() const { return isSingleShot; }

        void SetSingleShot(b8 singleShot) { this->isSingleShot = singleShot; }

        b8 IsActive() const { return isActive; }

		// - Signals -

        CE_SIGNAL(OnTimeOut);

    private:

        void Tick();

        clock_t prevTime = 0;
        f32 totalTimeTaken = 0.0f;

        b8 isActive = false;

        int milliseconds = 0;

        FIELD()
        b8 isSingleShot = false;

        friend class CApplication;
        friend class CWidget;
    };
    
} // namespace CE::Widgets

#include "CTimer.rtti.h"