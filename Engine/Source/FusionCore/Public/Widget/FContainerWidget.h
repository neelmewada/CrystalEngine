#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FContainerWidget : public FWidget
    {
        CE_CLASS(FContainerWidget, FWidget)
    public:

        FContainerWidget();

        u32 GetChildCount() const { return children.GetSize(); }

        FWidget* GetChild(u32 index) const { return children[index]; }

        void SetContextRecursively(FFusionContext* context) override;

        FWidget* HitTest(Vec2 localMousePos) override;

        bool ChildExistsRecursive(FWidget* child) override;

        void HandleEvent(FEvent* event) override;

        void InsertChild(int index, FWidget* child);

    protected:

        void ClearStyle() override;

        bool TryAddChild(FWidget* child) override;

        bool TryRemoveChild(FWidget* child) override;

    	void OnChildWidgetDestroyed(FWidget* child) override;

    protected: // - Fields -

        FIELD()
        Array<FWidget*> children{};

    protected: // - Fusion Fields -

        FIELD()
        bool m_ClipChildren = false;

    public: // - Fusion Properties -

        FUSION_PROPERTY(ClipChildren);


        FUSION_WIDGET;
    };

} // namespace CE

#include "FContainerWidget.rtti.h"