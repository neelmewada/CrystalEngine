#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CDockSpace::CDockSpace()
	{
        receiveMouseEvents = true;
        rootPadding = Vec4(1, 1, 1, 1) * 2.0f;

        CDockSplitView* full = CreateDefaultSubobject<CDockSplitView>("DockSplitView");
        full->dockSpace = this;
        full->splitRatio = 1.0f;

        dockSplits.Add(full);
	}

	CDockSpace::~CDockSpace()
	{
        if (nativeWindow)
        {
            nativeWindow->SetHitTestDelegate(nullptr);
        }
	}

    bool CDockSpace::Split(CDockSplitView* originalSplit, f32 splitRatio, CDockSplitDirection splitDirection)
    {
        splitRatio = Math::Clamp(splitRatio, 0.05f, 0.95f);

        CDockSplitView* first = CreateObject<CDockSplitView>(originalSplit, "DockSplitView");
        CDockSplitView* second = CreateObject<CDockSplitView>(originalSplit, "DockSplitView");

        first->splitRatio = 1.0f - splitRatio;
        second->splitRatio = splitRatio;
        first->dockSpace = second->dockSpace = this;

        originalSplit->splitDirection = splitDirection;

        return true;
    }

    bool CDockSpace::Split(f32 splitRatio, CDockSplitDirection splitDirection)
    {
        CDockSplitView* originalSplit = dockSplits.Top();
        return Split(originalSplit, splitRatio, splitDirection);
    }

    void CDockSpace::HandleEvent(CEvent* event)
    {
        // TODO: Handle events here
        if (!event->isConsumed && event->IsMouseEvent())
        {
            CE_LOG(Info, All, "{}", event->type);
        }

	    Super::HandleEvent(event);
    }

    void CDockSpace::OnPlatformWindowSet()
    {
        Super::OnPlatformWindowSet();

        nativeWindow->SetHitTestDelegate(MemberDelegate(&Self::WindowHitTest, this));
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
        else if (object->IsOfType<CDockSplitView>())
        {
            CDockSplitView* splitView = (CDockSplitView*)object;
            splitView->parent = this;
            attachedWidgets.Add(splitView);
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
        else if (object->IsOfType<CDockSplitView>())
        {
            CDockSplitView* splitView = (CDockSplitView*)object;
            splitView->parent = nullptr;
            attachedWidgets.Remove(splitView);
        }
	}

	void CDockSpace::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        PlatformWindow* nativeWindow = GetRootNativeWindow();

        u32 w = 0, h = 0;
        nativeWindow->GetDrawableWindowSize(&w, &h);

        // - Fetch Styles -

        // - Draw Background -

        Color bgColor = computedStyle.properties[CStylePropertyType::Background].color;
        
        CPen pen = CPen(Color::FromRGBA32(48, 48, 48));
        CBrush brush = CBrush(bgColor);//CBrush(Color::FromRGBA32(21, 21, 21));
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

        // Set tab height same as the top padding
        f32 majorTabHeight = 40.0f;//windowPadding.top;
        tabRects.Clear();

        // Only 1 DockSplitView supported in a Major DockSpace
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
                    if (i == 1)
                        brush.SetColor(bgColor);
                    painter->SetPen(pen);
                    painter->SetBrush(brush);
                    painter->SetFont(font);

                    Rect tabRect = Rect::FromSize(xOffset, 25.0f, Math::Min(tabTitleSize.width + 70, 270.0f), majorTabHeight);
                    painter->DrawRoundedRect(tabRect, Vec4(5, 5, 0, 0));
                    tabRects.Add(tabRect);

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

	}

    bool CDockSpace::WindowHitTest(PlatformWindow* window, Vec2 position)
    {
        for (const Rect& tabRect : tabRects)
        {
	        if (tabRect.Contains(position))
                return false;
        }

        if (position.y < 60)
            return true;
        return false;
    }

} // namespace CE::Widgets
