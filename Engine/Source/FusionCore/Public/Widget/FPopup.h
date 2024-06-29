#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FPopup : public FStyledWidget
    {
        CE_CLASS(FPopup, FStyledWidget)
    public:

        FPopup();

        // - Public API -

        void SetInitialSize(Vec2 size) { initialSize = size; }

        void ClosePopup();

    protected:

        void HandleEvent(FEvent* event) override;

    protected: // - Fusion Fields -

        FIELD()
        bool m_BlockInteraction = false;

        FIELD()
        bool m_AutoClose = true;


    public: // - Fusion Properties - 

        FUSION_PROPERTY(BlockInteraction);
        FUSION_PROPERTY(AutoClose);

    private:

        Vec2 initialPos;
        Vec2 initialSize;
        bool isNativePopup = false;
        bool isShown = false;

        FUSION_WIDGET;
    };
    
}


#include "FPopup.rtti.h"