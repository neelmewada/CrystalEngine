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

	// A primary data type
	enum class DataType
	{
		S8,       // signed char / char (with sensible compilers)
		U8,       // unsigned char
		S16,      // short
		U16,      // unsigned short
		S32,      // int
		U32,      // unsigned int
		S64,      // long long / __int64
		U64,      // unsigned long long / unsigned __int64
		Float,    // float
		Double,   // double
		COUNT
	};
	ENUM_CLASS_FLAGS(DataType);

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
		//Disabled             = 1 << 14,  // disable interactions -> use BeginDisabled() or ItemFlags_Disabled
		AlignTextBaseLine = 1 << 15,  // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
		NoKeyModifiers = 1 << 16,  // disable mouse interaction if a key modifier is held
		NoHoldingActiveId = 1 << 17,  // don't set ActiveId while holding the mouse (PressedOnClick only)
		NoNavFocus = 1 << 18,  // don't override navigation focus when activated (FIXME: this is essentially used everytime an item uses ItemFlags_NoNav, but because legacy specs don't requires LastItemData to be set ButtonBehavior(), we can't poll g.LastItemData.InFlags)
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

	enum TableFlags
	{
		// Features
		TableFlags_None = 0,
		TableFlags_Resizable = 1 << 0,   // Enable resizing columns.
		TableFlags_Reorderable = 1 << 1,   // Enable reordering columns in header row (need calling TableSetupColumn() + TableHeadersRow() to display headers)
		TableFlags_Hideable = 1 << 2,   // Enable hiding/disabling columns in context menu.
		TableFlags_Sortable = 1 << 3,   // Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see TableFlags_SortMulti and TableFlags_SortTristate.
		TableFlags_NoSavedSettings = 1 << 4,   // Disable persisting columns order, width and sort settings in the .ini file.
		TableFlags_ContextMenuInBody = 1 << 5,   // Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
		// Decorations
		TableFlags_RowBg = 1 << 6,   // Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
		TableFlags_BordersInnerH = 1 << 7,   // Draw horizontal borders between rows.
		TableFlags_BordersOuterH = 1 << 8,   // Draw horizontal borders at the top and bottom.
		TableFlags_BordersInnerV = 1 << 9,   // Draw vertical borders between columns.
		TableFlags_BordersOuterV = 1 << 10,  // Draw vertical borders on the left and right sides.
		TableFlags_BordersH = TableFlags_BordersInnerH | TableFlags_BordersOuterH, // Draw horizontal borders.
		TableFlags_BordersV = TableFlags_BordersInnerV | TableFlags_BordersOuterV, // Draw vertical borders.
		TableFlags_BordersInner = TableFlags_BordersInnerV | TableFlags_BordersInnerH, // Draw inner borders.
		TableFlags_BordersOuter = TableFlags_BordersOuterV | TableFlags_BordersOuterH, // Draw outer borders.
		TableFlags_Borders = TableFlags_BordersInner | TableFlags_BordersOuter,   // Draw all borders.
		TableFlags_NoBordersInBody = 1 << 11,  // [ALPHA] Disable vertical borders in columns Body (borders will always appear in Headers). -> May move to style
		TableFlags_NoBordersInBodyUntilResize = 1 << 12,  // [ALPHA] Disable vertical borders in columns Body until hovered for resize (borders will always appear in Headers). -> May move to style
		// Sizing Policy (read above for defaults)
		TableFlags_SizingFixedFit = 1 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
		TableFlags_SizingFixedSame = 2 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable TableFlags_NoKeepColumnsVisible.
		TableFlags_SizingStretchProp = 3 << 13,  // Columns default to _WidthStretch with default weights proportional to each columns contents widths.
		TableFlags_SizingStretchSame = 4 << 13,  // Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
		// Sizing Extra Options
		TableFlags_NoHostExtendX = 1 << 16,  // Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
		TableFlags_NoHostExtendY = 1 << 17,  // Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
		TableFlags_NoKeepColumnsVisible = 1 << 18,  // Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
		TableFlags_PreciseWidths = 1 << 19,  // Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
		// Clipping
		TableFlags_NoClip = 1 << 20,  // Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
		// Padding
		TableFlags_PadOuterX = 1 << 21,  // Default if BordersOuterV is on. Enable outermost padding. Generally desirable if you have headers.
		TableFlags_NoPadOuterX = 1 << 22,  // Default if BordersOuterV is off. Disable outermost padding.
		TableFlags_NoPadInnerX = 1 << 23,  // Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
		// Scrolling
		TableFlags_ScrollX = 1 << 24,  // Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this creates a child window, ScrollY is currently generally recommended when using ScrollX.
		TableFlags_ScrollY = 1 << 25,  // Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
		// Sorting
		TableFlags_SortMulti = 1 << 26,  // Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).
		TableFlags_SortTristate = 1 << 27,  // Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).

		// [Internal] Combinations and masks
		TableFlags_SizingMask_ = TableFlags_SizingFixedFit | TableFlags_SizingFixedSame | TableFlags_SizingStretchProp | TableFlags_SizingStretchSame,
	};
	ENUM_CLASS_FLAGS(TableFlags);

	enum TableColumnFlags
	{
		// Input configuration flags
		TableColumnFlags_None = 0,
		TableColumnFlags_Disabled = 1 << 0,   // Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
		TableColumnFlags_DefaultHide = 1 << 1,   // Default as a hidden/disabled column.
		TableColumnFlags_DefaultSort = 1 << 2,   // Default as a sorting column.
		TableColumnFlags_WidthStretch = 1 << 3,   // Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
		TableColumnFlags_WidthFixed = 1 << 4,   // Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
		TableColumnFlags_NoResize = 1 << 5,   // Disable manual resizing.
		TableColumnFlags_NoReorder = 1 << 6,   // Disable manual reordering this column, this will also prevent other columns from crossing over this column.
		TableColumnFlags_NoHide = 1 << 7,   // Disable ability to hide/disable this column.
		TableColumnFlags_NoClip = 1 << 8,   // Disable clipping for this column (all NoClip columns will render in a same draw command).
		TableColumnFlags_NoSort = 1 << 9,   // Disable ability to sort on this field (even if ImGuiTableFlags_Sortable is set on the table).
		TableColumnFlags_NoSortAscending = 1 << 10,  // Disable ability to sort in the ascending direction.
		TableColumnFlags_NoSortDescending = 1 << 11,  // Disable ability to sort in the descending direction.
		TableColumnFlags_NoHeaderLabel = 1 << 12,  // TableHeadersRow() will not submit label for this column. Convenient for some small columns. Name will still appear in context menu.
		TableColumnFlags_NoHeaderWidth = 1 << 13,  // Disable header text width contribution to automatic column width.
		TableColumnFlags_PreferSortAscending = 1 << 14,  // Make the initial sort direction Ascending when first sorting on this column (default).
		TableColumnFlags_PreferSortDescending = 1 << 15,  // Make the initial sort direction Descending when first sorting on this column.
		TableColumnFlags_IndentEnable = 1 << 16,  // Use current Indent value when entering cell (default for column 0).
		TableColumnFlags_IndentDisable = 1 << 17,  // Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.

		// Output status flags, read-only via TableGetColumnFlags()
		TableColumnFlags_IsEnabled = 1 << 24,  // Status: is enabled == not hidden by user/api (referred to as "Hide" in _DefaultHide and _NoHide) flags.
		TableColumnFlags_IsVisible = 1 << 25,  // Status: is visible == is enabled AND not clipped by scrolling.
		TableColumnFlags_IsSorted = 1 << 26,  // Status: is currently part of the sort specs
		TableColumnFlags_IsHovered = 1 << 27,  // Status: is hovered by mouse

		// [Internal] Combinations and masks
		TableColumnFlags_WidthMask_ = TableColumnFlags_WidthStretch | TableColumnFlags_WidthFixed,
		TableColumnFlags_IndentMask_ = TableColumnFlags_IndentEnable | TableColumnFlags_IndentDisable,
		TableColumnFlags_StatusMask_ = TableColumnFlags_IsEnabled | TableColumnFlags_IsVisible | TableColumnFlags_IsSorted | TableColumnFlags_IsHovered,
		TableColumnFlags_NoDirectResize_ = 1 << 30,  // [Internal] Disable user resizing this column directly (it may however we resized indirectly from its left edge)

	};
	ENUM_CLASS_FLAGS(TableColumnFlags);

	enum TableRowFlags
	{
		TableRowFlags_None = 0,
		TableRowFlags_Headers = 1 << 0,   // Identify header row (set default background color + width of its contents accounted differently for auto column width)
	};

	enum NavHighlightFlags
	{
		NavHighlightFlags_None = 0,
		NavHighlightFlags_TypeDefault = 1 << 0,
		NavHighlightFlags_TypeThin = 1 << 1,
		NavHighlightFlags_AlwaysDraw = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
		NavHighlightFlags_NoRounding = 1 << 3,
	};
	ENUM_CLASS_FLAGS(NavHighlightFlags);

	// Flags used by upcoming items
	// - input: PushItemFlag() manipulates g.CurrentItemFlags, ItemAdd() calls may add extra flags.
	// - output: stored in g.LastItemData.InFlags
	// Current window shared by all windows.
	// This is going to be exposed in imgui.h when stabilized enough.
	enum ItemFlags
	{
		// Controlled by user
		ItemFlags_None = 0,
		ItemFlags_NoTabStop = 1 << 0,  // false     // Disable keyboard tabbing. This is a "lighter" version of ItemFlags_NoNav.
		ItemFlags_ButtonRepeat = 1 << 1,  // false     // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
		ItemFlags_Disabled = 1 << 2,  // false     // Disable interactions but doesn't affect visuals. See BeginDisabled()/EndDisabled(). See github.com/ocornut/imgui/issues/211
		ItemFlags_NoNav = 1 << 3,  // false     // Disable any form of focusing (keyboard/gamepad directional navigation and SetKeyboardFocusHere() calls)
		ItemFlags_NoNavDefaultFocus = 1 << 4,  // false     // Disable item being a candidate for default focus (e.g. used by title bar items)
		ItemFlags_SelectableDontClosePopup = 1 << 5,  // false     // Disable MenuItem/Selectable() automatically closing their popup window
		ItemFlags_MixedValue = 1 << 6,  // false     // [BETA] Represent a mixed/indeterminate value, generally multi-selection where values differ. Currently only supported by Checkbox() (later should support all sorts of widgets)
		ItemFlags_ReadOnly = 1 << 7,  // false     // [ALPHA] Allow hovering interactions but underlying value is not changed.
		ItemFlags_NoWindowHoverableCheck = 1 << 8,  // false     // Disable hoverable check in ItemHoverable()

		// Controlled by widget code
		ItemFlags_Inputable = 1 << 10, // false     // [WIP] Auto-activate input mode when tab focused. Currently only used and supported by a few items before it becomes a generic feature.
	};
	ENUM_CLASS_FLAGS(ItemFlags);

	enum TextInputFlags
	{
		TextInputFlags_None = 0,
		TextInputFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-*/
		TextInputFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
		TextInputFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
		TextInputFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
		TextInputFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
		TextInputFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
		TextInputFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
		TextInputFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
		TextInputFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
		TextInputFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
		TextInputFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
		TextInputFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
		TextInputFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
		TextInputFlags_AlwaysOverwrite = 1 << 13,  // Overwrite mode
		TextInputFlags_ReadOnly = 1 << 14,  // Read-only mode
		TextInputFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
		TextInputFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
		TextInputFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
		TextInputFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
		TextInputFlags_CallbackEdit = 1 << 19,  // Callback on any edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)
		TextInputFlags_EscapeClearsAll = 1 << 20,  // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
		
		
		TextInputFlags_Multiline = 1 << 26,  // For internal use by InputTextMultiline()
		TextInputFlags_NoMarkEdited = 1 << 27,  // For internal use by functions using InputText() before reformatting data
		TextInputFlags_MergedItem = 1 << 28,  // For internal use by TempInputText(), will skip calling ItemAdd(). Require bounding-box to strictly match.

	};
	ENUM_CLASS_FLAGS(TextInputFlags);

	struct TextInputCallbackData
	{
		void*		   ctx;            // Parent UI context
		TextInputFlags eventFlag;      // One ImGuiInputTextFlags_Callback*    // Read-only
		TextInputFlags flags;          // What user passed to InputText()      // Read-only
		void*		   userData;       // What user passed to InputText()      // Read-only

		// Arguments for the different callback events
		// - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
		// - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
		unsigned short eventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
		int            eventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
		char*		   buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
		int            bufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator storage. In C land: == strlen(some_text), in C++ land: string.length()
		int            bufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator storage. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
		bool           bufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
		int            cursorPos;      //                                      // Read-write   // [Completion,History,Always]
		int            selectionStart; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
		int            selectionEnd;   //       

	};

	typedef int (*TextInputCallback)(TextInputCallbackData* data);

	enum SliderFlags
	{
		SliderFlags_None = 0,
		SliderFlags_AlwaysClamp = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
		SliderFlags_Logarithmic = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using SliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
		SliderFlags_NoRoundToFormat = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
		SliderFlags_NoInput = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget
		SliderFlags_InvalidMask_ = 0x7000000F,   // [Internal] We treat using those bits as being potentially a 'float power' argument from the previous API that has got miscast to this enum, and will trigger an assert if needed.
	};
	ENUM_CLASS_FLAGS(SliderFlags);

	// Flags for BeginTabBar()
	enum TabBarFlags
	{
		TabBarFlags_None = 0,
		TabBarFlags_Reorderable = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
		TabBarFlags_AutoSelectNewTabs = 1 << 1,   // Automatically select new tabs when they appear
		TabBarFlags_TabListPopupButton = 1 << 2,   // Disable buttons to open the tab list popup
		TabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
		TabBarFlags_NoTabListScrollingButtons = 1 << 4,   // Disable scrolling buttons (apply when fitting policy is TabBarFlags_FittingPolicyScroll)
		TabBarFlags_NoTooltip = 1 << 5,   // Disable tooltips when hovering a tab
		TabBarFlags_FittingPolicyResizeDown = 1 << 6,   // Resize tabs when they don't fit
		TabBarFlags_FittingPolicyScroll = 1 << 7,   // Add scroll buttons when tabs don't fit
		TabBarFlags_FittingPolicyMask_ = TabBarFlags_FittingPolicyResizeDown | TabBarFlags_FittingPolicyScroll,
		TabBarFlags_FittingPolicyDefault_ = TabBarFlags_FittingPolicyResizeDown,

		TabBarFlags_DockNode = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
		TabBarFlags_IsFocused = 1 << 21,
		TabBarFlags_SaveSettings = 1 << 22,  // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs

	};
	ENUM_CLASS_FLAGS(TabBarFlags);

	// Flags for BeginTabItem()
	enum TabItemFlags
	{
		TabItemFlags_None = 0,
		TabItemFlags_UnsavedDocument = 1 << 0,   // Display a dot next to the title + tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
		TabItemFlags_SetSelected = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
		TabItemFlags_NoCloseWithMiddleMouseButton = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
		TabItemFlags_NoPushId = 1 << 3,   // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
		TabItemFlags_NoTooltip = 1 << 4,   // Disable tooltip for the given tab
		TabItemFlags_NoReorder = 1 << 5,   // Disable reordering this tab or having another tab cross over this tab
		TabItemFlags_Leading = 1 << 6,   // Enforce the tab position to the left of the tab bar (after the tab list popup button)
		TabItemFlags_Trailing = 1 << 7,   // Enforce the tab position to the right of the tab bar (before the scrolling buttons)
	};
	ENUM_CLASS_FLAGS(TabItemFlags);

	enum SelectableFlags
	{
		SelectableFlags_None = 0,
		SelectableFlags_DontClosePopups = 1 << 0,   // Clicking this doesn't close parent popup window
		SelectableFlags_SpanAllColumns = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
		SelectableFlags_AllowDoubleClick = 1 << 2,   // Generate press events on double clicks too
		SelectableFlags_Disabled = 1 << 3,   // Cannot be selected, display grayed out text
		SelectableFlags_AllowItemOverlap = 1 << 4,   // (WIP) Hit testing to allow subsequent widgets to overlap this one
	};
	ENUM_CLASS_FLAGS(SelectableFlags);

	struct COREGUI_API GuiStyleState
	{
		// Border
		Vec4 borderRadius{};
		f32 borderThickness = 0;
		Color borderColor{};
		// Color
		Color background{};
		Color foreground = Color::White();
		// Text
		TextAlign textAlign = TextAlign_MiddleCenter;
		u32 fontSize = 16;
		Name fontName{};
	};
    
} // namespace CE

