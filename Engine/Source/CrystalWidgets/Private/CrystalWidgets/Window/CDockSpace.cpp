#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CDockSpace::CDockSpace()
	{
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

    bool CDockSpace::Split(CDockSplitView* originalSplit, f32 splitRatio, CDockSplitDirection splitDirection)
    {
        if (dockType == CDockType::Major)
            return false;

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
        if (dockType == CDockType::Major)
            return false;

        CDockSplitView* originalSplit = dockSplits.Top();
        return Split(originalSplit, splitRatio, splitDirection);
    }

    void CDockSpace::HandleEvent(CEvent* event)
    {
        
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

        if (position.y < 60) // Add width offset for menu bar: (position.x > 256)
            return true;
        return false;
    }

} // namespace CE::Widgets
