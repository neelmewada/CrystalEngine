#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTimer : public Object
    {
        CE_CLASS(FTimer, Object)
    public:

        FTimer();
        virtual ~FTimer();

        void Reset();

        void Start(int milliseconds);

        void Start();

        void Restart();

        void Stop();

        b8 IsSingleShot() const { return isSingleShot; }

        void SetSingleShot(b8 singleShot) { this->isSingleShot = singleShot; }

        b8 IsActive() const { return isActive; }

        // - Events -

    private:

        FIELD()
        FVoidEvent m_OnTimeOut{};

        void Tick();

    public:

        FUSION_EVENT(OnTimeOut);

    private:

        clock_t prevTime = 0;
        f32 totalTimeTaken = 0.0f;

        b8 isActive = false;

        int milliseconds = 0;

        FIELD()
        b8 isSingleShot = false;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FTimer.rtti.h"