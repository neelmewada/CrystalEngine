#include "CrystalWidgets.h"

namespace CE::Widgets
{
	CDockSplitView::CDockSplitView()
	{

	}

	CDockSplitView::~CDockSplitView()
	{

	}

	CDockSplitView* CDockSplitView::FindSplit(Name splitName)
	{
        if (GetName() == splitName)
            return this;

        for (auto child : children)
        {
            CDockSplitView* result = child->FindSplit(splitName);
            if (result)
            {
                return result;
            }
        }

        return nullptr;
	}

	void CDockSplitView::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CDockSplitView>())
        {
            children.Add((CDockSplitView*)subobject);
            ((CDockSplitView*)subobject)->parentSplitView = this;
        }
	}

	void CDockSplitView::OnSubobjectDetached(Object* subobject)
	{
        Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CDockSplitView>())
        {
            children.Remove((CDockSplitView*)subobject);
            ((CDockSplitView*)subobject)->parentSplitView = nullptr;
        }
	}

	CDockSpace::CDockSpace()
	{
        dockSplits.Clear();

        CDockSplitView* full = CreateDefaultSubobject<CDockSplitView>("Split");
        full->splitRatio = 1.0f;

        dockSplits.Add(full);
	}

	CDockSpace::~CDockSpace()
	{
        
	}

    bool CDockSpace::Split(CDockSplitView* originalSplit, f32 splitRatio, CDockSplitDirection splitDirection)
    {
        splitRatio = Math::Clamp(splitRatio, 0.05f, 0.95f);

        CDockSplitView* first = CreateObject<CDockSplitView>(originalSplit, "Split");
        CDockSplitView* second = CreateObject<CDockSplitView>(originalSplit, "Split");

        first->splitRatio = 1.0f - splitRatio;
        second->splitRatio = splitRatio;

        originalSplit->splitDirection = splitDirection;

        return true;
    }

    bool CDockSpace::Split(f32 splitRatio, CDockSplitDirection splitDirection)
    {
        CDockSplitView* originalSplit = dockSplits.Top();
        return Split(originalSplit, splitRatio, splitDirection);
    }

	void CDockSpace::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

        if (!object)
            return;

        if (object->IsOfType<CWindow>())
        {
            CWindow* window = (CWindow*)object;

            dockSplits.Top()->AddSubWidget(window);
        }
	}

	void CDockSpace::OnSubobjectDetached(Object* object)
	{
        Super::OnSubobjectDetached(object);

        if (!object)
            return;

        if (object->IsOfType<CWindow>())
        {
            CWindow* window = (CWindow*)object;

            for (CDockSplitView* dockSplit : dockSplits)
            {
                dockSplit->RemoveSubWidget(window);
            }
        }
	}

	void CDockSpace::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        // TODO: Remove dependency on nativeWindow

        PlatformWindow* nativeWindow = GetRootNativeWindow();

        u32 w = 0, h = 0;
        nativeWindow->GetDrawableWindowSize(&w, &h);

        // - Draw Background -

        CPen pen = CPen(Color::FromRGBA32(48, 48, 48));
        CBrush brush = CBrush(Color::FromRGBA32(21, 21, 21));
        CFont font = CFont("Roboto", 15, false);

        painter->SetBrush(brush);
        f32 windowEdgeSize = 0;

        if (this->nativeWindow != nullptr && this->nativeWindow->IsBorderless())
        {
            windowEdgeSize = 2.0f;
            pen.SetWidth(windowEdgeSize);
            painter->SetPen(pen);
        }

        painter->DrawRect(Rect::FromSize(0, 0, w, h));

        // - Draw Tabs -

        constexpr f32 majorTabHeight = 40.0f;

        if (dockType == CDockType::Major && dockSplits.NonEmpty())
        {
            CDockSplitView* split = dockSplits[0];

            f32 xOffset = 20.0f;

            for (int i = 0; i < split->GetSubWidgetCount(); ++i)
            {
                CWidget* subWidget = split->GetSubWidget(i);

                if (subWidget->IsOfType<CDockWindow>())
                {
                    CDockWindow* dockWindow = (CDockWindow*)subWidget;

                    Vec2 tabTitleSize = painter->CalculateTextSize(dockWindow->GetTitle());

                    pen.SetWidth(0.0f);
                    pen.SetColor(Color::Clear());
                    brush.SetColor(Color::FromRGBA32(36, 36, 36));
                    painter->SetPen(pen);
                    painter->SetBrush(brush);
                    painter->SetFont(font);

                    Rect tabRect = Rect::FromSize(xOffset, 2.5f, Math::Min(tabTitleSize.width + 70, 270.0f), majorTabHeight);
                    painter->DrawRoundedRect(tabRect, Vec4(5, 5, 0, 0));
                    //painter->DrawRect(Rect::FromSize(windowEdgeSize, tabRect.bottom, w - windowEdgeSize * 2, 40));

                    pen.SetColor(Color::White());
                    painter->SetPen(pen);

                    painter->DrawText(dockWindow->GetTitle(), tabRect + Rect(15, tabRect.GetSize().height / 2 - tabTitleSize.height / 2, 0, 0));

                    xOffset += tabRect.GetSize().width + 2.5f;
                }
            }
        }
        else if (dockType == CDockType::Minor)
        {
	        
        }

        // - Draw Contents -

        if (dockType == CDockType::Major && dockSplits.NonEmpty())
        {
            CDockSplitView* split = dockSplits[0];

            painter->PushChildCoordinateSpace(Vec2(0, majorTabHeight));
            {
                pen.SetWidth(0.0f); pen.SetColor(Color::Clear());
            	brush.SetColor(Color::FromRGBA32(36, 36, 36));
                painter->SetPen(pen);
                painter->SetBrush(brush);


            }
            painter->PopChildCoordinateSpace();
        }
	}

} // namespace CE::Widgets
