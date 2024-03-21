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
            CDockSplitView* splitView = (CDockSplitView*)subobject;
            splitView->dockSpace = dockSpace;
            children.Add(splitView);
            splitView->parentSplitView = this;
            rootPadding = Vec4(0, 0, 0, 0);
        }
        else if (subobject->IsOfType<CDockWindow>())
        {
            CDockWindow* dockWindow = (CDockWindow*)subobject;
            dockWindow->dockSpace = dockSpace;
            rootPadding = Vec4(0, 40, 0, 0);
        }
	}

	void CDockSplitView::OnSubobjectDetached(Object* subobject)
	{
        Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CDockSplitView>())
        {
            CDockSplitView* splitView = (CDockSplitView*)subobject;
            splitView->dockSpace = nullptr;
            children.Remove(splitView);
            splitView->parentSplitView = nullptr;
        }
        else if (subobject->IsOfType<CDockWindow>())
        {
            CDockWindow* dockWindow = (CDockWindow*)subobject;
            dockWindow->dockSpace = nullptr;
        }
	}

	CDockSpace::CDockSpace()
	{
        rootPadding = Vec4(1, 1, 1, 1) * 2.0f;

        CDockSplitView* full = CreateDefaultSubobject<CDockSplitView>("DockSplitView");
        full->dockSpace = this;
        full->splitRatio = 1.0f;

        dockSplits.Add(full);
	}

	CDockSpace::~CDockSpace()
	{
        
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

        // Set tab height same as the top padding
        f32 majorTabHeight = 40.0f;//windowPadding.top;

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
