
#include "CoreRHI.h"

namespace CE
{

    IMGUIStyle::IMGUIStyle()
	{
        alpha = 1.0f;             // Global alpha applies to everything in Dear ImGui.
        disabledAlpha = 0.60f;            // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
        windowPadding = Vec2(8, 8);      // Padding within a window
        windowRounding = 0.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
        windowBorderSize = 1.0f;             // Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
        windowMinSize = Vec2(32, 32);    // Minimum window size
        windowTitleAlign = Vec2(0.0f, 0.5f);// Alignment for title bar text
        windowMenuButtonPosition = IMGUIDir::Left;    // Position of the collapsing/docking button in the title bar (left/right). Defaults to ImGuiDir_Left.
        childRounding = 0.0f;             // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
        childBorderSize = 1.0f;             // Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
        popupRounding = 0.0f;             // Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
        popupBorderSize = 1.0f;             // Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
        framePadding = Vec2(4, 3);      // Padding within a framed rectangle (used by most widgets)
        frameRounding = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
        frameBorderSize = 0.0f;             // Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
        itemSpacing = Vec2(8, 4);      // Horizontal and vertical spacing between widgets/lines
        itemInnerSpacing = Vec2(4, 4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
        cellPadding = Vec2(4, 2);      // Padding within a table cell
        touchExtraPadding = Vec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
        indentSpacing = 21.0f;            // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
        columnsMinSpacing = 6.0f;             // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
        scrollbarSize = 14.0f;            // Width of the vertical scrollbar, Height of the horizontal scrollbar
        scrollbarRounding = 9.0f;             // Radius of grab corners rounding for scrollbar
        grabMinSize = 12.0f;            // Minimum width/height of a grab box for slider/scrollbar
        grabRounding = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
        logSliderDeadzone = 4.0f;             // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
        tabRounding = 4.0f;             // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
        tabBorderSize = 0.0f;             // Thickness of border around tabs.
        tabMinWidthForCloseButton = 0.0f;           // Minimum width for close button to appear on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
        colorButtonPosition = IMGUIDir::Right;   // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
        buttonTextAlign = Vec2(0.5f, 0.5f);// Alignment of button text when button is larger than text.
        selectableTextAlign = Vec2(0.0f, 0.0f);// Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
        separatorTextBorderSize = 3.0f;             // Thickkness of border in SeparatorText()
        separatorTextAlign = Vec2(0.0f, 0.5f);// Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
        separatorTextPadding = Vec2(20.0f, 3.f);// Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.
        displayWindowPadding = Vec2(19, 19);    // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
        displaySafeAreaPadding = Vec2(3, 3);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
        mouseCursorScale = 1.0f;             // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
        antiAliasedLines = true;             // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU.
        antiAliasedLinesUseTex = true;             // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering).
        antiAliasedFill = true;             // Enable anti-aliased filled shapes (rounded rectangles, circles, etc.).
        curveTessellationTol = 1.25f;            // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
        circleTessellationMaxError = 0.30f;         // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.

	}

} // namespace CE

