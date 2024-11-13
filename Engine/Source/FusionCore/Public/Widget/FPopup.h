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

        virtual void OnPopupClosed();

        bool IsShown() const { return isShown; }

    protected:

        void HandleEvent(FEvent* event) override;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(bool, BlockInteraction);
        FUSION_PROPERTY(bool, AutoClose);

    private:

        Vec2 initialPos;
        Vec2 initialSize;
        Vec2 controlSize;
        bool isNativePopup = false;
        bool isShown = false;
        bool positionFound = false;

        FUSION_WIDGET;
    };
    
}


#include "FPopup.rtti.h"