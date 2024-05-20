# Crystal Widgets

The crystal widgets module is a widget gui library that is used by the Crystal Editor itself.
It exists in the engine domain, which means it can be used by runtime standalone builds too.

Crystal widgets lets you create widgets in C++ and use CSS for styling and flex layout. It uses the 2D renderer from the CoreRPI to draw the widgets.

All widgets derive from the CWidget class, including CWindow. And all windows derive from CWindow class.

## Event handling

The CWidget class has a virtual function called `void HandleEvent(CEvent* event)` which is called by the widgets event system. 
Currently, there are a few different type of events, like mouse events, focus events, key events, drag events and paint event.

## Image button example

Below example creates an image button.

```c++
// MyImageButton.h

CLASS()
class MyImageButton : public CWidget
{
    CE_CLASS(MyImageButton, CWidget)
public:

    void Construct() override
    {
        icon = CreateObject<CImage>(this, "Icon");
        icon->SetBackgroundImage("/Editor/Assets/Icons/Arrow");

        label = CreateObject<CLabel>(this, "Label");
        label->SetText("Click Me");
    }

    void HandleEvent(CEvent* event)
    {
        if (event->type == CEventType::MouseRelease)
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;
            if (mouseEvent->isInside)
                CE_LOG(Info, All, "Button clicked!");
        }
    }

    FIELD()
    CImage* icon = nullptr;

    FIELD()
    CLabel* label = nullptr;
};

#include "MyImageButton.rtti.h" // auto generated rtti header
```
Style.css
```css
MyImageButton {
    flex-direction: row;
    align-items: center;
    column-gap: 5px;
    padding: 10px 10px;
    background-color: rgb(36, 36, 36);
}

MyImageButton:hovered {
    background-color: rgb(60, 60, 60);
}

MyImageButton:pressed {
    background-color: rgb(26, 26, 26);
}
```


