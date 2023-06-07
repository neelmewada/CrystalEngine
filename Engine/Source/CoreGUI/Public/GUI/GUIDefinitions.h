#pragma once

namespace CE::GUI
{
    typedef unsigned int ID;

    enum WindowFlags
    {
        WF_None = 0,
        WF_NoTitleBar = 1 << 0,   // Disable title-bar
        WF_NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
        WF_NoMove = 1 << 2,   // Disable user moving the window
        WF_NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        WF_NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        WF_NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        WF_AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
        WF_NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        WF_NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
        WF_NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        WF_MenuBar = 1 << 10,  // Has a menu-bar
        WF_HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(Vec2(width,0.0f)); prior to calling Begin() to specify width. Read code in _demo in the "Horizontal Scrolling" section.
        WF_NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        WF_NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        WF_AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        WF_AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        WF_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
        WF_NoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
        WF_NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        WF_UnsavedDocument = 1 << 20,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        WF_NoDocking = 1 << 21,  // Disable docking of this window

        WF_NoNav = WF_NoNavInputs | WF_NoNavFocus,
        WF_NoDecoration = WF_NoTitleBar | WF_NoResize | WF_NoScrollbar | WF_NoCollapse,
        WF_NoInputs = WF_NoMouseInputs | WF_NoNavInputs | WF_NoNavFocus,
        WF_FullScreen = WF_NoTitleBar | WF_NoCollapse | WF_NoResize | WF_NoMove | WF_NoBringToFrontOnFocus | WF_NoNavFocus,

        // [Internal]
        WF_NavFlattened = 1 << 23,  // [BETA] On child window: allow gamepad/keyboard navigation to cross over parent border to this child or between sibling child windows.
        WF_ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
        WF_Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
        WF_Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
        WF_Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
        WF_ChildMenu = 1 << 28,  // Don't use! For internal use by BeginMenu()
        WF_DockNodeHost = 1 << 29,  // Don't use! For internal use by Begin()/NewFrame()

        // Custom
        WF_NoPadding = 1 << 30
    };
    ENUM_CLASS_FLAGS(WindowFlags);

    enum Dir
    {
        Dir_None = -1,
        Dir_Left = 0,
        Dir_Right = 1,
        Dir_Up = 2,
        Dir_Down = 3,
        Dir_COUNT
    };
    ENUM_CLASS_FLAGS(Dir);

    struct CORERHI_API Style
    {
        float       alpha;                      // Global alpha applies to everything in Dear .
        float       disabledAlpha;              // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
        Vec2        windowPadding;              // Padding within a window.
        float       windowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
        float       windowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Vec2        windowMinSize;              // Minimum window size. This is a global setting. If you want to constrain individual windows, use SetNextWindowSizeConstraints().
        Vec2        windowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
        Dir    windowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to Dir_Left.
        float       childRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
        float       childBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        float       popupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
        float       popupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Vec2        framePadding;               // Padding within a framed rectangle (used by most widgets).
        float       frameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
        float       frameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Vec2        itemSpacing;                // Horizontal and vertical spacing between widgets/lines.
        Vec2        itemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
        Vec2        cellPadding;                // Padding within a table cell
        Vec2        touchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
        float       indentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
        float       columnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
        float       scrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
        float       scrollbarRounding;          // Radius of grab corners for scrollbar.
        float       grabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
        float       grabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
        float       logSliderDeadzone;          // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
        float       tabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
        float       tabBorderSize;              // Thickness of border around tabs.
        float       tabMinWidthForCloseButton;  // Minimum width for close button to appear on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
        Dir    colorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to Dir_Right.
        Vec2        buttonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
        Vec2        selectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
        float       separatorTextBorderSize;    // Thickkness of border in SeparatorText()
        Vec2        separatorTextAlign;         // Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
        Vec2        separatorTextPadding;       // Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.
        Vec2        displayWindowPadding;       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
        Vec2        displaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
        float       mouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later.
        bool        antiAliasedLines;           // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
        bool        antiAliasedLinesUseTex;     // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering). Latched at the beginning of the frame (copied to ImDrawList).
        bool        antiAliasedFill;            // Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
        float       curveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
        float       circleTessellationMaxError; // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.

        Style();
    };

    enum TreeNodeFlags
    {
        TNF_None = 0,
        TNF_Selected = 1 << 0,   // Draw as selected
        TNF_Framed = 1 << 1,   // Draw frame with background (e.g. for CollapsingHeader)
        TNF_AllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
        TNF_NoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
        TNF_NoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
        TNF_DefaultOpen = 1 << 5,   // Default node to be open
        TNF_OpenOnDoubleClick = 1 << 6,   // Need double-click to open node
        TNF_OpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
        TNF_Leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
        TNF_Bullet = 1 << 9,   // Display a bullet instead of arrow
        TNF_FramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
        TNF_SpanAvailWidth = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
        TNF_SpanFullWidth = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
        TNF_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
        //TNF_NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
        TNF_CollapsingHeader = TNF_Framed | TNF_NoTreePushOnOpen | TNF_NoAutoOpenOnLog,
        TNF_LeafFlags = TNF_Leaf | TNF_NoTreePushOnOpen,
    };
    ENUM_CLASS_FLAGS(TreeNodeFlags);


    enum HoveredFlags
    {
        Hovered_None = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
        Hovered_ChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
        Hovered_RootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
        Hovered_AnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
        Hovered_NoPopupHierarchy = 1 << 3,   // IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        Hovered_DockHierarchy = 1 << 4,   // IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        Hovered_AllowWhenBlockedByPopup = 1 << 5,   // Return true even if a popup window is normally blocking access to this item/window
        //Hovered_AllowWhenBlockedByModal     = 1 << 6,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
        Hovered_AllowWhenBlockedByActiveItem = 1 << 7,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
        Hovered_AllowWhenOverlapped = 1 << 8,   // IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window
        Hovered_AllowWhenDisabled = 1 << 9,   // IsItemHovered() only: Return true even if the item is disabled
        Hovered_NoNavOverride = 1 << 10,  // Disable using gamepad/keyboard navigation state when active, always query mouse.
        Hovered_RectOnly = Hovered_AllowWhenBlockedByPopup | Hovered_AllowWhenBlockedByActiveItem | Hovered_AllowWhenOverlapped,
        Hovered_RootAndChildWindows = Hovered_RootWindow | Hovered_ChildWindows,
        
        // Hovering delays (for tooltips)
        Hovered_DelayNormal = 1 << 11,  // Return true after io.HoverDelayNormal elapsed (~0.30 sec)
        Hovered_DelayShort = 1 << 12,  // Return true after io.HoverDelayShort elapsed (~0.10 sec)
        Hovered_NoSharedDelay = 1 << 13,  // Disable shared delay system where moving from one item to the next keeps the previous timer for a short time (standard for tooltips with long delays)
    };
    ENUM_CLASS_FLAGS(HoveredFlags);

    enum class MouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        COUNT = 5
    };
    ENUM_CLASS_FLAGS(MouseButton);

    enum StyleCol
    {
        StyleCol_Text,
        StyleCol_TextDisabled,
        StyleCol_WindowBg,              // Background of normal windows
        StyleCol_ChildBg,               // Background of child windows
        StyleCol_PopupBg,               // Background of popups, menus, tooltips windows
        StyleCol_Border,
        StyleCol_BorderShadow,
        StyleCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
        StyleCol_FrameBgHovered,
        StyleCol_FrameBgActive,
        StyleCol_TitleBg,
        StyleCol_TitleBgActive,
        StyleCol_TitleBgCollapsed,
        StyleCol_MenuBarBg,
        StyleCol_ScrollbarBg,
        StyleCol_ScrollbarGrab,
        StyleCol_ScrollbarGrabHovered,
        StyleCol_ScrollbarGrabActive,
        StyleCol_CheckMark,
        StyleCol_SliderGrab,
        StyleCol_SliderGrabActive,
        StyleCol_Button,
        StyleCol_ButtonHovered,
        StyleCol_ButtonActive,
        StyleCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
        StyleCol_HeaderHovered,
        StyleCol_HeaderActive,
        StyleCol_Separator,
        StyleCol_SeparatorHovered,
        StyleCol_SeparatorActive,
        StyleCol_ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
        StyleCol_ResizeGripHovered,
        StyleCol_ResizeGripActive,
        StyleCol_Tab,                   // TabItem in a TabBar
        StyleCol_TabHovered,
        StyleCol_TabActive,
        StyleCol_TabUnfocused,
        StyleCol_TabUnfocusedActive,
        StyleCol_DockingPreview,        // Preview overlay color when about to docking something
        StyleCol_DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
        StyleCol_PlotLines,
        StyleCol_PlotLinesHovered,
        StyleCol_PlotHistogram,
        StyleCol_PlotHistogramHovered,
        StyleCol_TableHeaderBg,         // Table header background
        StyleCol_TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
        StyleCol_TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
        StyleCol_TableRowBg,            // Table row background (even rows)
        StyleCol_TableRowBgAlt,         // Table row background (odd rows)
        StyleCol_TextSelectedBg,
        StyleCol_DragDropTarget,        // Rectangle highlighting a drop target
        StyleCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
        StyleCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
        StyleCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
        StyleCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
        StyleCol_COUNT
    };
    ENUM_CLASS_FLAGS(StyleCol);

    enum StyleVar
    {
        StyleVar_Alpha,               // float     Alpha
        StyleVar_DisabledAlpha,       // float     DisabledAlpha
        StyleVar_WindowPadding,       // Vec2    WindowPadding
        StyleVar_WindowRounding,      // float     WindowRounding
        StyleVar_WindowBorderSize,    // float     WindowBorderSize
        StyleVar_WindowMinSize,       // Vec2    WindowMinSize
        StyleVar_WindowTitleAlign,    // Vec2    WindowTitleAlign
        StyleVar_ChildRounding,       // float     ChildRounding
        StyleVar_ChildBorderSize,     // float     ChildBorderSize
        StyleVar_PopupRounding,       // float     PopupRounding
        StyleVar_PopupBorderSize,     // float     PopupBorderSize
        StyleVar_FramePadding,        // Vec2    FramePadding
        StyleVar_FrameRounding,       // float     FrameRounding
        StyleVar_FrameBorderSize,     // float     FrameBorderSize
        StyleVar_ItemSpacing,         // Vec2    ItemSpacing
        StyleVar_ItemInnerSpacing,    // Vec2    ItemInnerSpacing
        StyleVar_IndentSpacing,       // float     IndentSpacing
        StyleVar_CellPadding,         // Vec2    CellPadding
        StyleVar_ScrollbarSize,       // float     ScrollbarSize
        StyleVar_ScrollbarRounding,   // float     ScrollbarRounding
        StyleVar_GrabMinSize,         // float     GrabMinSize
        StyleVar_GrabRounding,        // float     GrabRounding
        StyleVar_TabRounding,         // float     TabRounding
        StyleVar_ButtonTextAlign,     // Vec2    ButtonTextAlign
        StyleVar_SelectableTextAlign, // Vec2    SelectableTextAlign
        StyleVar_SeparatorTextBorderSize,// float  SeparatorTextBorderSize
        StyleVar_SeparatorTextAlign,  // Vec2    SeparatorTextAlign
        StyleVar_SeparatorTextPadding,// Vec2    SeparatorTextPadding
        StyleVar_COUNT
    };
    ENUM_CLASS_FLAGS(StyleVar);

    enum class ButtonFlags
    {
        None = 0,
        MouseButtonLeft = 1 << 0,   // React on left mouse button (default)
        MouseButtonRight = 1 << 1,   // React on right mouse button
        MouseButtonMiddle = 1 << 2,   // React on center mouse button

        // [Internal]
        MouseButtonMask_ = MouseButtonLeft | MouseButtonRight | MouseButtonMiddle,
        MouseButtonDefault_ = MouseButtonLeft,

		PressedOnClick = 1 << 4,   // return true on click (mouse down event)
		PressedOnClickRelease = 1 << 5,   // [Default] return true on click + release on same item <-- this is what the majority of Button are using
		PressedOnClickReleaseAnywhere = 1 << 6, // return true on click + release even if the release event is not done while hovering the item
		PressedOnRelease = 1 << 7,   // return true on release (default requires click+release)
		PressedOnDoubleClick = 1 << 8,   // return true on double-click (default requires click+release)
		PressedOnDragDropHold = 1 << 9,   // return true when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
		Repeat = 1 << 10,  // hold to repeat
		FlattenChildren = 1 << 11,  // allow interactions even if a child window is overlapping
		AllowItemOverlap = 1 << 12,  // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
		DontClosePopups = 1 << 13,  // disable automatically closing parent popup on press // [UNUSED]
		//Disabled             = 1 << 14,  // disable interactions -> use BeginDisabled() or ImGuiItemFlags_Disabled
		AlignTextBaseLine = 1 << 15,  // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
		NoKeyModifiers = 1 << 16,  // disable mouse interaction if a key modifier is held
		NoHoldingActiveId = 1 << 17,  // don't set ActiveId while holding the mouse (PressedOnClick only)
		NoNavFocus = 1 << 18,  // don't override navigation focus when activated (FIXME: this is essentially used everytime an item uses ImGuiItemFlags_NoNav, but because legacy specs don't requires LastItemData to be set ButtonBehavior(), we can't poll g.LastItemData.InFlags)
		NoHoveredOnFocus = 1 << 19,  // don't report as hovered when nav focus is on this item
		NoSetKeyOwner = 1 << 20,  // don't set key/input owner on the initial click (note: mouse buttons are keys! often, the key in question will be ImGuiKey_MouseLeft!)
		NoTestKeyOwner = 1 << 21,  // don't test key/input owner when polling the key (note: mouse buttons are keys! often, the key in question will be ImGuiKey_MouseLeft!)
		PressedOnMask_ = PressedOnClick | PressedOnClickRelease | PressedOnClickReleaseAnywhere | PressedOnRelease | PressedOnDoubleClick | PressedOnDragDropHold,
		PressedOnDefault_ = PressedOnClickRelease,

    };
    ENUM_CLASS_FLAGS(ButtonFlags);

	enum TextAlign
	{
		TextAlign_Inherited,
		TextAlign_TopLeft,
		TextAlign_TopCenter,
		TextAlign_TopRight,
		TextAlign_MiddleLeft,
		TextAlign_MiddleCenter,
		TextAlign_MiddleRight,
		TextAlign_BottomLeft,
		TextAlign_BottomCenter,
		TextAlign_BottomRight,
	};
	ENUM_CLASS_FLAGS(TextAlign);

	enum FocusFlags
	{
		FOCUS_None = 0,
		FOCUS_ChildWindows = 1 << 0,   // Return true if any children of the window is focused
		FOCUS_RootWindow = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
		FOCUS_AnyWindow = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
		FOCUS_NoPopupHierarchy = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
		FOCUS_DockHierarchy = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
		FOCUS_RootAndChildWindows = FOCUS_RootWindow | FOCUS_ChildWindows,
	};
	ENUM_CLASS_FLAGS(FocusFlags);

    enum class Cond
    {
        None = 0,        // No condition (always set the variable), same as _Always
        Always = 1 << 0,   // No condition (always set the variable), same as _None
        Once = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
        FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
        Appearing = 1 << 3,   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
    };
    ENUM_CLASS_FLAGS(Cond);

    // Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
    struct SizeCallbackData
    {
        void* userData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
        Vec2  pos;            // Read-only.   Window position, for reference.
        Vec2  currentSize;    // Read-only.   Current window size.
        Vec2  desiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
    };

    typedef void (*SizeCallback)(SizeCallbackData* Data); // Callback function for SetNextWindowSizeConstraints()

#ifndef ImTextureID
    typedef void* ImTextureID;          // Default: store a pointer or an integer fitting in a pointer (most renderer backends are ok with that)
#endif


    enum DockFlags
    {
        DockFlags_None = 0,
        DockFlags_KeepAliveOnly = 1 << 0,   // Shared       // Don't display the dockspace node but keep it alive. Windows docked into this dockspace node won't be undocked.
        //DockFlags_NoCentralNode              = 1 << 1,   // Shared       // Disable Central Node (the node which can stay empty)
        DockFlags_NoDockingInCentralNode = 1 << 2,   // Shared       // Disable docking inside the Central Node, which will be always kept empty.
        DockFlags_PassthruCentralNode = 1 << 3,   // Shared       // Enable passthru dockspace: 1) DockSpace() will render a ImGuiCol_WindowBg background covering everything excepted the Central Node when empty. Meaning the host window should probably use SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When Central Node is empty: let inputs pass-through + won't display a DockingEmptyBg background. See demo for details.
        DockFlags_NoSplit = 1 << 4,   // Shared/Local // Disable splitting the node into smaller nodes. Useful e.g. when embedding dockspaces into a main root one (the root one may have splitting disabled to reduce confusion). Note: when turned off, existing splits will be preserved.
        DockFlags_NoResize = 1 << 5,   // Shared/Local // Disable resizing node using the splitter/separators. Useful with programmatically setup dockspaces.
        DockFlags_AutoHideTabBar = 1 << 6,   // Shared/Local // Tab bar will automatically hide when there is a single window in the dock node.
    };
    ENUM_CLASS_FLAGS(DockFlags);

    // Flags stored in Viewport::Flags, giving indications to the platform backends.
    enum ViewportFlags
    {
        Viewport_None = 0,
        Viewport_IsPlatformWindow = 1 << 0,   // Represent a Platform Window
        Viewport_IsPlatformMonitor = 1 << 1,   // Represent a Platform Monitor (unused yet)
        Viewport_OwnedByApp = 1 << 2,   // Platform Window: Was created/managed by the user application? (rather than our backend)
        Viewport_NoDecoration = 1 << 3,   // Platform Window: Disable platform decorations: title bar, borders, etc. (generally set all windows, but if ConfigFlags_ViewportsDecoration is set we only set this on popups/tooltips)
        Viewport_NoTaskBarIcon = 1 << 4,   // Platform Window: Disable platform task bar icon (generally set on popups/tooltips, or all windows if ConfigFlags_ViewportsNoTaskBarIcon is set)
        Viewport_NoFocusOnAppearing = 1 << 5,   // Platform Window: Don't take focus when created.
        Viewport_NoFocusOnClick = 1 << 6,   // Platform Window: Don't take focus when clicked on.
        Viewport_NoInputs = 1 << 7,   // Platform Window: Make mouse pass through so we can drag this window while peaking behind it.
        Viewport_NoRendererClear = 1 << 8,   // Platform Window: Renderer doesn't need to clear the framebuffer ahead (because we will fill it entirely).
        Viewport_NoAutoMerge = 1 << 9,   // Platform Window: Avoid merging this window into another host window. This can only be set via WindowClass viewport flags override (because we need to now ahead if we are going to create a viewport in the first place!).
        Viewport_TopMost = 1 << 10,  // Platform Window: Display on top (for tooltips only).
        Viewport_CanHostOtherWindows = 1 << 11,  // Viewport can host multiple  windows (secondary viewports are associated to a single window). // FIXME: In practice there's still probably code making the assumption that this is always and only on the MainViewport. Will fix once we add support for "no main viewport".
        
        // Output status flags (from Platform)
        Viewport_IsMinimized = 1 << 12,  // Platform Window: Window is minimized, can skip render. When minimized we tend to avoid using the viewport pos/size for clipping window or testing if they are contained in the viewport.
        Viewport_IsFocused = 1 << 13,  // Platform Window: Window is focused (last call to Platform_GetWindowFocus() returned true)
    };
    ENUM_CLASS_FLAGS(ViewportFlags);

    struct Viewport
    {
        ID             id;                     // Unique identifier for the viewport
        ViewportFlags  flags;                  // See Viewport_
        Vec2              pos;                    // Main Area: Position of the viewport (Dear  coordinates are the same as OS desktop/native coordinates)
        Vec2              size;                   // Main Area: Size of the viewport.
        Vec2              workPos;                // Work Area: Position of the viewport minus task bars, menus bars, status bars (>= Pos)
        Vec2              workSize;               // Work Area: Size of the viewport minus task bars, menu bars, status bars (<= Size)
        float               dpiScale;               // 1.0f = 96 DPI = No extra scale.
        ID             parentViewportId;       // (Advanced) 0: no parent. Instruct the platform backend to setup a parent/child relationship between platform windows.
        void* drawData;               // The ImDrawData corresponding to this viewport. Valid after Render() and until the next call to NewFrame().D

        // Platform/Backend Dependent Data
        // Our design separate the Renderer and Platform backends to facilitate combining default backends with each others.
        // When our create your own backend for a custom engine, it is possible that both Renderer and Platform will be handled
        // by the same system and you may not need to use all the UserData/Handle fields.
        // The library never uses those fields, they are merely storage to facilitate backend implementation.
        void* rendererUserData;       // void* to hold custom data structure for the renderer (e.g. swap chain, framebuffers etc.). generally set by your Renderer_CreateWindow function.
        void* platformUserData;       // void* to hold custom data structure for the OS / platform (e.g. windowing info, render context). generally set by your Platform_CreateWindow function.
        void* platformHandle;         // void* for FindViewportByPlatformHandle(). (e.g. suggested to use natural platform handle such as HWND, GLFWWindow*, SDL_Window*)
        void* platformHandleRaw;      // void* to hold lower-level, platform-native window handle (under Win32 this is expected to be a HWND, unused for other platforms), when using an abstraction layer like GLFW or SDL (where PlatformHandle would be a SDL_Window*)
        bool                platformWindowCreated;  // Platform window has been created (Platform_CreateWindow() has been called). This is false during the first frame where a viewport is being created.
        bool                platformRequestMove;    // Platform window requested move (e.g. window was moved by the OS / host window manager, authoritative position will be OS window position)
        bool                platformRequestResize;  // Platform window requested resize (e.g. window was resized by the OS / host window manager, authoritative size will be OS window size)
        bool                platformRequestClose;   // Platform window requested closure (e.g. window was moved by the OS / host window manager, e.g. pressing ALT-F4)

        Viewport() { memset(this, 0, sizeof(*this)); }
        ~Viewport() {  }

        // Helpers
        Vec2              GetCenter() const { return Vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f); }
        Vec2              GetWorkCenter() const { return Vec2(workPos.x + workSize.x * 0.5f, workPos.y + workSize.y * 0.5f); }
    };

	struct StyleColor
	{
		StyleColor() 
		{}

		StyleColor(const Color& color) : isColor(true), color(color)
		{}

		StyleColor(const Gradient& gradient) : isGradient(true), gradient(gradient)
		{}

		bool IsEmpty() const
		{
			return !isColor && !isGradient;
		}

		Color color;
		b8 isColor = false;

		Gradient gradient;
		b8 isGradient = false;
	};
    
} // namespace CE

