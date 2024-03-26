#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CDockSpace::CDockSpace()
	{
        allowVerticalScroll = allowHorizontalScroll = false;
        receiveMouseEvents = false;
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

    bool CDockSpace::Split(CDockSplitView* originalSplit, f32 splitRatio, CDockSplitDirection splitDirection, Name splitName1, Name splitName2)
    {
        if (dockType == CDockType::Major)
            return false;

        splitRatio = Math::Clamp(splitRatio, 0.05f, 0.95f);

        if (!splitName1.IsValid())
            splitName1 = "DockSplitView";
        if (!splitName2.IsValid())
            splitName2 = "DockSplitView";

        CDockSplitView* first = CreateObject<CDockSplitView>(originalSplit, splitName1.GetString());
        CDockSplitView* second = CreateObject<CDockSplitView>(originalSplit, splitName2.GetString());

        first->splitRatio = 1.0f - splitRatio;
        second->splitRatio = splitRatio;
        first->dockSpace = second->dockSpace = this;

        originalSplit->splitDirection = splitDirection;

        return true;
    }

    bool CDockSpace::Split(f32 splitRatio, CDockSplitDirection splitDirection, Name splitName1, Name splitName2)
    {
        if (dockType == CDockType::Major)
            return false;

        CDockSplitView* originalSplit = dockSplits.Top();
        return Split(originalSplit, splitRatio, splitDirection, splitName1, splitName2);
    }

    void CDockSpace::HandleEvent(CEvent* event)
    {
        
	    Super::HandleEvent(event);
    }

    void CDockSpace::OnPlatformWindowSet()
    {
        Super::OnPlatformWindowSet();

        if (nativeWindow)
        {
	        nativeWindow->SetHitTestDelegate(MemberDelegate(&Self::WindowHitTest, this));
        }
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

        Color bgColor = computedStyle.properties[CStylePropertyType::Background].color;

        CPen pen = CPen(Color::RGBA8(60, 60, 60));
        CBrush brush = CBrush(Color::Clear());
        CFont font = CFont("Roboto", 15, false);

        if (GetDockType() == CDockType::Major)
        {
            PlatformWindow* nativeWindow = GetRootNativeWindow();

            u32 w = 0, h = 0;
            nativeWindow->GetDrawableWindowSize(&w, &h);

            // - Fetch Styles -

            // - Draw Background -

            painter->SetBrush(brush);
            f32 windowEdgeSize = 0;

            if (this->nativeWindow != nullptr && this->nativeWindow->IsBorderless())
            {
                windowEdgeSize = 2.0f;
                pen.SetWidth(windowEdgeSize);
                painter->SetPen(pen);
            }

            painter->DrawRect(Rect::FromSize(0, 0, w, h));
        }
        else if (GetDockType() == CDockType::Minor)
        {
            Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());

            painter->SetBrush(brush);
            f32 windowEdgeSize = 0;

            painter->DrawRect(rect);
        }
	}

    bool CDockSpace::WindowHitTest(PlatformWindow* window, Vec2 position)
    {
        if (dockSplits.IsEmpty()) // Should never happen
        {
            if (position.y < 60)
                return true;
            return false;
        }

        for (const auto& tab : dockSplits[0]->tabs)
        {
	        if (tab.rect.Contains(position))
                return false;
        }

        if (controlRects.NonEmpty() && nativeWindow != nullptr && PlatformMisc::GetCurrentPlatform() != PlatformName::Mac)
        {
            if (canBeMinimized && position.x >= controlRects[0].min.x && position.y < 60)
                return false;
            if (canBeMaximized && position.x >= controlRects[1].min.x && position.y < 60)
                return false;
            if (canBeClosed && position.x >= controlRects[2].min.x && position.y < 60)
                return false;
        }

        if (position.y < 60) // Add width offset for menu bar: (position.x > 256)
            return true;
        return false;
    }

} // namespace CE::Widgets
