# Fusion Widgets

Fusion is a declarative C++ widget library that powers the Crystal Editor itself using a builtin 2D renderer. In future, it will be modified to be used for game's GUI.

Fusion lets you create widgets in C++ using declarative syntax and apply flexible styling on them.

## Features

* Declarative UI structure.
* Texture support: Use textures to draw icons and images with custom scaling and tiling.
* Fast 2D renderer: Uses builtin 2D renderer that uses instancing to draw almost all widgets in a single draw call.
* Styling: Flexible styling using `FStyle` and `FStyleSet`.
* 2D clipping.
* Flexible layout: Use layouts like FStackBox, FOverlayStack, FCompoundWidget, FSplitBox, etc to create complex layouts.

## Example

Here is a window that displays some Fusion widgets:

```c++
namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SampleWidgetWindow : public FToolWindow
    {
        CE_CLASS(SampleWidgetWindow, FToolWindow)
    protected:

        SampleWidgetWindow();

        void Construct() override;

        void OnBeforeDestroy() override;

    public: // - Public API -

    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SampleWidgetWindow.rtti.h"

```
