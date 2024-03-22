#include "CrystalWidgets.h"

namespace CE::Widgets
{
    constexpr f32 majorTabHeight = 40.0f;
    constexpr f32 majorTabOffset = 25.0f;

    CDockSplitView::CDockSplitView()
    {
        receiveDragEvents = true;
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

    bool CDockSplitView::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        if (!subWidgetClass)
            return false;
        if (subWidgetClass->IsSubclassOf<CDockWindow>())
            return true;
        if (dockSpace->GetDockType() == CDockType::Minor && subWidgetClass->IsSubclassOf<CDockSplitView>())
            return true;
        return false;
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
            dockWindow->SetEnabled(selectedTab == dockedWindows.GetSize());
            dockedWindows.Add(dockWindow);
            selectedTab = 0;
            if (dockSpace->GetDockType() == CDockType::Major)
                rootPadding = Vec4(0, 60, 0, 0);
            else
                rootPadding = Vec4(0, 30, 0, 0);
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
            dockedWindows.Remove(dockWindow);
        }
    }

    void CDockSplitView::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent() || event->IsDragEvent())
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;

            if (!event->isConsumed && dockSpace->GetDockType() == CDockType::Major)
            {
                PlatformWindow* platformWindow = dockSpace->GetRootNativeWindow();
                Vec2 windowPos = platformWindow->GetWindowPosition().ToVec2();
                Vec2i windowSize = platformWindow->GetWindowSize();
                platformWindow->GetWindowSize();
                Vec2 pos = mouseEvent->mousePos - windowPos;
                Vec2 prevPos = mouseEvent->prevMousePos - windowPos;
                Vec2 delta = pos - prevPos;
                Rect screenSpaceRect = GetScreenSpaceRect();

                if (event->type == CEventType::DragBegin && event->sender == this)
                {
                    for (int i = 0; i < tabs.GetSize(); ++i)
                    {
                        if (tabs[i].rect.Contains(pos))
                        {
                            if (selectedTab != i)
                            {
                                SetNeedsPaint();
                            }
                            selectedTab = i;
                            event->Consume(this); // The user clicked inside a tab
                            break;
                        }
                    }

                    for (int i = 0; i < tabs.GetSize(); ++i)
                    {
                        GetSubWidget(i)->SetEnabled(selectedTab == i);
                    }
                }
                else if (event->type == CEventType::DragMove)
                {
                    CDragEvent* dragEvent = (CDragEvent*)event;

                    if (tabs.GetSize() == 1)
                    {
                        platformWindow->SetWindowPosition(Vec2i(windowPos.x + delta.x, windowPos.y + delta.y));
                        dragEvent->Consume(this);
                    }
                    else if (tabs.GetSize() > 1)
                    {
                        for (int i = 0; i < tabs.GetSize(); ++i)
                        {
                            if (i == selectedTab)
                            {
                                Vec2 size = tabs[i].rect.GetSize();
                                tabs[i].rect.min.x += delta.x;
                                tabs[i].rect.max = tabs[i].rect.min + size;
                                SetNeedsPaint();
                            }
                        }

                        bool canDetach = !dockedWindows[selectedTab]->IsMainWindow();
                        dragEvent->Consume(this);

                        if (canDetach && (pos.y < 0 || pos.y > majorTabOffset + majorTabHeight + 5))
                        {
                            // Detach the window from this DockSpace
                            CDockWindow* dockWindow = dockedWindows[selectedTab];
                            RemoveSubWidget(dockWindow);

                            PlatformWindow* newWindow = PlatformApplication::Get()->
                        		CreatePlatformWindow(dockWindow->GetName().GetString(), windowSize.width, windowSize.height, false, false);
                            newWindow->SetBorderless(true);
                            newWindow->SetWindowPosition(Vec2i(mouseEvent->mousePos.x - 50, mouseEvent->mousePos.y - majorTabHeight));

                            CDockSpace* newDockSpace = CreateWindow<CDockSpace>(dockWindow->GetName().GetString(), nullptr, newWindow);
                            newDockSpace->AddSubWidget(dockWindow);
                            
                            selectedTab = 0;

                            tabs.Clear();
                            SetNeedsStyle();
                            SetNeedsPaint();

                            dragEvent->draggedWidget = newDockSpace->GetLastDockSplit();
                        }
                    }
                }
                else if (event->type == CEventType::DragEnd)
                {
                    event->Consume(this);

                    tabs.Clear();
                    SetNeedsPaint();
                    CDragEvent* dragEvent = (CDragEvent*)event;
                }
            }
        }

	    Super::HandleEvent(event);
    }

    void CDockSplitView::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Color bgColor = computedStyle.properties[CStylePropertyType::Background].color;

        CPen pen = CPen(Color::FromRGBA32(48, 48, 48));
        CBrush brush = CBrush(bgColor);//CBrush(Color::FromRGBA32(21, 21, 21));
        CFont font = CFont("Roboto", 15, false);

        // - Draw Tabs -

        // Only 1 DockSplitView supported in a Major DockSpace
        if (dockSpace->GetDockType() == CDockType::Major)
        {
            f32 xOffset = 20.0f;

            int tabsCount = tabs.GetSize();
            if (tabsCount < GetSubWidgetCount())
            {
                tabs.Resize(GetSubWidgetCount());

                for (int i = 0; i < tabs.GetSize(); ++i)
                {
                    GetSubWidget(i)->SetEnabled(selectedTab == i);
                }
            }

            // Draw non-selected tabs first
            for (int i = 0; i < GetSubWidgetCount(); ++i)
            {
                CWidget* subWidget = GetSubWidget(i);

                if (subWidget->IsOfType<CDockWindow>())
                {
                    CDockWindow* dockWindow = (CDockWindow*)subWidget;

                    Vec2 tabTitleSize = painter->CalculateTextSize(dockWindow->GetTitle());

                    pen.SetWidth(0.0f);
                    pen.SetColor(Color::Clear());
                    brush.SetColor(Color::FromRGBA32(36, 36, 36));
                    if (i != selectedTab)
                        brush.SetColor(bgColor);
                    painter->SetPen(pen); painter->SetBrush(brush); painter->SetFont(font);

                    if (i >= tabsCount)
                    {
                        Rect rect = Rect::FromSize(xOffset, majorTabOffset, Math::Min(tabTitleSize.width + 70, 270.0f), majorTabHeight);
                        tabs[i] = { rect };
                    }

                    Rect tabRect = tabs[i].rect;

                    if (selectedTab != i)
                    {
                        painter->DrawRoundedRect(tabRect, Vec4(5, 5, 0, 0));

                        pen.SetColor(Color::White());
                        painter->SetPen(pen);

                        painter->DrawText(dockWindow->GetTitle(), tabRect + Rect(15, tabRect.GetSize().height / 2 - tabTitleSize.height / 2, 0, 0));
                    }

                    xOffset += tabRect.GetSize().width + 2.5f;
                }
            }

            // Draw selected tab at last
            xOffset = 20.0f;

            for (int i = 0; i < GetSubWidgetCount(); ++i)
            {
                CWidget* subWidget = GetSubWidget(i);

                if (subWidget->IsOfType<CDockWindow>())
                {
                    CDockWindow* dockWindow = (CDockWindow*)subWidget;

                    Vec2 tabTitleSize = painter->CalculateTextSize(dockWindow->GetTitle());

                    pen.SetWidth(0.0f);
                    pen.SetColor(Color::Clear());
                    brush.SetColor(Color::FromRGBA32(36, 36, 36));
                    if (i != selectedTab)
                    	brush.SetColor(bgColor);
                    painter->SetPen(pen); painter->SetBrush(brush); painter->SetFont(font);

                    if (i >= tabsCount)
                    {
                        Rect rect = Rect::FromSize(xOffset, majorTabOffset, Math::Min(tabTitleSize.width + 70, 270.0f), majorTabHeight);
                        tabs[i] = { rect };
                    }

                    Rect tabRect = tabs[i].rect;
                    auto size = tabRect.GetSize();
                    if (tabRect.min.x < 20)
                    {
                        tabRect.min.x = 20;
                        tabRect.max.x = tabRect.min.x + size.x;
                    }

                    if (selectedTab == i)
                    {
                        painter->DrawRoundedRect(tabRect, Vec4(5, 5, 0, 0));

                        pen.SetColor(Color::White());
                        painter->SetPen(pen);

                        painter->DrawText(dockWindow->GetTitle(), tabRect + Rect(15, tabRect.GetSize().height / 2 - tabTitleSize.height / 2, 0, 0));
                    }

                    xOffset += tabRect.GetSize().width + 2.5f;
                }
            }

        }
        else if (dockSpace->GetDockType() == CDockType::Minor)
        {

        }
    }

} // namespace CE::Widgets
