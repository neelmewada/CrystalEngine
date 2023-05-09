#pragma once

namespace CE
{
    typedef unsigned int ImGuiID;

    enum class ImGuiWindowFlags
    {
        None = 0,
        NoTitleBar = 1 << 0,   // Disable title-bar
        NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
        NoMove = 1 << 2,   // Disable user moving the window
        NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
        NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
        NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        MenuBar = 1 << 10,  // Has a menu-bar
        HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(Vec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
        NoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
        NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        UnsavedDocument = 1 << 20,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        NoDocking = 1 << 21,  // Disable docking of this window

        NoNav = NoNavInputs | NoNavFocus,
        NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
        NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus,

        // [Internal]
        NavFlattened = 1 << 23,  // [BETA] On child window: allow gamepad/keyboard navigation to cross over parent border to this child or between sibling child windows.
        ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
        Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
        Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
        Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
        ChildMenu = 1 << 28,  // Don't use! For internal use by BeginMenu()
        DockNodeHost = 1 << 29,  // Don't use! For internal use by Begin()/NewFrame()
    };
    ENUM_CLASS_FLAGS(ImGuiWindowFlags);

    enum class ImGuiColor
    {
        Text,
        TextDisabled,
        WindowBg,              // Background of normal windows
        ChildBg,               // Background of child windows
        PopupBg,               // Background of popups, menus, tooltips windows
        Border,
        BorderShadow,
        FrameBg,               // Background of checkbox, radio button, plot, slider, text input
        FrameBgHovered,
        FrameBgActive,
        TitleBg,
        TitleBgActive,
        TitleBgCollapsed,
        MenuBarBg,
        ScrollbarBg,
        ScrollbarGrab,
        ScrollbarGrabHovered,
        ScrollbarGrabActive,
        CheckMark,
        SliderGrab,
        SliderGrabActive,
        Button,
        ButtonHovered,
        ButtonActive,
        Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
        HeaderHovered,
        HeaderActive,
        Separator,
        SeparatorHovered,
        SeparatorActive,
        ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
        ResizeGripHovered,
        ResizeGripActive,
        Tab,                   // TabItem in a TabBar
        TabHovered,
        TabActive,
        TabUnfocused,
        TabUnfocusedActive,
        DockingPreview,        // Preview overlay color when about to docking something
        DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
        PlotLines,
        PlotLinesHovered,
        PlotHistogram,
        PlotHistogramHovered,
        TableHeaderBg,         // Table header background
        TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
        TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
        TableRowBg,            // Table row background (even rows)
        TableRowBgAlt,         // Table row background (odd rows)
        TextSelectedBg,
        DragDropTarget,        // Rectangle highlighting a drop target
        NavHighlight,          // Gamepad/keyboard: current highlighted item
        NavWindowingHighlight, // Highlight window when using CTRL+TAB
        NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
        ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
        COUNT
    };
    ENUM_CLASS_FLAGS(ImGuiColor);


    enum class ImGuiDir
    {
        None = -1,
        Left = 0,
        Right = 1,
        Up = 2,
        Down = 3,
        COUNT
    };
    ENUM_CLASS_FLAGS(ImGuiDir);

    struct ImGuiFontDesc
    {
        void* rawData = nullptr;
        SIZE_T byteSize = 0;
        u32 pointSize = 14;

        void* outFontHandle = nullptr;
    };

    struct ImGuiFontPreloadConfig
    {
        u32 preloadFontCount = 0;
        ImGuiFontDesc* preloadFonts = nullptr;
    };

    struct CORERHI_API ImGuiStyle
    {
        float       alpha;                      // Global alpha applies to everything in Dear ImGui.
        float       disabledAlpha;              // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
        Vec2        windowPadding;              // Padding within a window.
        float       windowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
        float       windowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Vec2        windowMinSize;              // Minimum window size. This is a global setting. If you want to constrain individual windows, use SetNextWindowSizeConstraints().
        Vec2        windowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
        ImGuiDir    windowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
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
        ImGuiDir    colorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
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

        ImGuiStyle();
    };

    enum class ImGuiTreeNodeFlags
    {
        None = 0,
        Selected = 1 << 0,   // Draw as selected
        Framed = 1 << 1,   // Draw frame with background (e.g. for CollapsingHeader)
        AllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
        NoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
        NoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
        DefaultOpen = 1 << 5,   // Default node to be open
        OpenOnDoubleClick = 1 << 6,   // Need double-click to open node
        OpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
        Leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
        Bullet = 1 << 9,   // Display a bullet instead of arrow
        FramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
        SpanAvailWidth = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
        SpanFullWidth = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
        NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
        //NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
        CollapsingHeader = Framed | NoTreePushOnOpen | NoAutoOpenOnLog,
        LeafFlags = Leaf | NoTreePushOnOpen,
    };
    ENUM_CLASS_FLAGS(ImGuiTreeNodeFlags);


    enum class ImGuiHoveredFlags
    {
        None = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
        ChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
        RootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
        AnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
        NoPopupHierarchy = 1 << 3,   // IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        DockHierarchy = 1 << 4,   // IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        AllowWhenBlockedByPopup = 1 << 5,   // Return true even if a popup window is normally blocking access to this item/window
        //AllowWhenBlockedByModal     = 1 << 6,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
        AllowWhenBlockedByActiveItem = 1 << 7,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
        AllowWhenOverlapped = 1 << 8,   // IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window
        AllowWhenDisabled = 1 << 9,   // IsItemHovered() only: Return true even if the item is disabled
        NoNavOverride = 1 << 10,  // Disable using gamepad/keyboard navigation state when active, always query mouse.
        RectOnly = AllowWhenBlockedByPopup | AllowWhenBlockedByActiveItem | AllowWhenOverlapped,
        RootAndChildWindows = RootWindow | ChildWindows,

        // Hovering delays (for tooltips)
        DelayNormal = 1 << 11,  // Return true after io.HoverDelayNormal elapsed (~0.30 sec)
        DelayShort = 1 << 12,  // Return true after io.HoverDelayShort elapsed (~0.10 sec)
        NoSharedDelay = 1 << 13,  // Disable shared delay system where moving from one item to the next keeps the previous timer for a short time (standard for tooltips with long delays)
    };
    ENUM_CLASS_FLAGS(ImGuiHoveredFlags);

    enum class ImGuiMouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        COUNT = 5
    };
    ENUM_CLASS_FLAGS(ImGuiMouseButton);

    enum class ImGuiStyleVar
    {
        Alpha,               // float     Alpha
        DisabledAlpha,       // float     DisabledAlpha
        WindowPadding,       // Vec2    WindowPadding
        WindowRounding,      // float     WindowRounding
        WindowBorderSize,    // float     WindowBorderSize
        WindowMinSize,       // Vec2    WindowMinSize
        WindowTitleAlign,    // Vec2    WindowTitleAlign
        ChildRounding,       // float     ChildRounding
        ChildBorderSize,     // float     ChildBorderSize
        PopupRounding,       // float     PopupRounding
        PopupBorderSize,     // float     PopupBorderSize
        FramePadding,        // Vec2    FramePadding
        FrameRounding,       // float     FrameRounding
        FrameBorderSize,     // float     FrameBorderSize
        ItemSpacing,         // Vec2    ItemSpacing
        ItemInnerSpacing,    // Vec2    ItemInnerSpacing
        IndentSpacing,       // float     IndentSpacing
        CellPadding,         // Vec2    CellPadding
        ScrollbarSize,       // float     ScrollbarSize
        ScrollbarRounding,   // float     ScrollbarRounding
        GrabMinSize,         // float     GrabMinSize
        GrabRounding,        // float     GrabRounding
        TabRounding,         // float     TabRounding
        ButtonTextAlign,     // Vec2    ButtonTextAlign
        SelectableTextAlign, // Vec2    SelectableTextAlign
        SeparatorTextBorderSize,// float  SeparatorTextBorderSize
        SeparatorTextAlign,  // Vec2    SeparatorTextAlign
        SeparatorTextPadding,// Vec2    SeparatorTextPadding
        COUNT
    };
    ENUM_CLASS_FLAGS(ImGuiStyleVar);

    enum class ImGuiButtonFlags
    {
        None = 0,
        MouseButtonLeft = 1 << 0,   // React on left mouse button (default)
        MouseButtonRight = 1 << 1,   // React on right mouse button
        MouseButtonMiddle = 1 << 2,   // React on center mouse button

        // [Internal]
        MouseButtonMask_ = MouseButtonLeft | MouseButtonRight | MouseButtonMiddle,
        MouseButtonDefault_ = MouseButtonLeft,
    };
    ENUM_CLASS_FLAGS(ImGuiButtonFlags);

    enum class ImGuiCond
    {
        None = 0,        // No condition (always set the variable), same as _Always
        Always = 1 << 0,   // No condition (always set the variable), same as _None
        Once = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
        FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
        Appearing = 1 << 3,   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
    };
    ENUM_CLASS_FLAGS(ImGuiCond);

    // Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
    struct ImGuiSizeCallbackData
    {
        void* userData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
        Vec2  pos;            // Read-only.   Window position, for reference.
        Vec2  currentSize;    // Read-only.   Current window size.
        Vec2  desiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
    };

    typedef void (*FImGuiSizeCallback)(ImGuiSizeCallbackData* Data); // Callback function for ImGuiSetNextWindowSizeConstraints()

#ifndef ImTextureID
    typedef void* ImTextureID;          // Default: store a pointer or an integer fitting in a pointer (most renderer backends are ok with that)
#endif


    // Flags stored in ImGuiViewport::Flags, giving indications to the platform backends.
    enum class ImGuiViewportFlags
    {
        None = 0,
        IsPlatformWindow = 1 << 0,   // Represent a Platform Window
        IsPlatformMonitor = 1 << 1,   // Represent a Platform Monitor (unused yet)
        OwnedByApp = 1 << 2,   // Platform Window: Was created/managed by the user application? (rather than our backend)
        NoDecoration = 1 << 3,   // Platform Window: Disable platform decorations: title bar, borders, etc. (generally set all windows, but if ImGuiConfigFlags_ViewportsDecoration is set we only set this on popups/tooltips)
        NoTaskBarIcon = 1 << 4,   // Platform Window: Disable platform task bar icon (generally set on popups/tooltips, or all windows if ImGuiConfigFlags_ViewportsNoTaskBarIcon is set)
        NoFocusOnAppearing = 1 << 5,   // Platform Window: Don't take focus when created.
        NoFocusOnClick = 1 << 6,   // Platform Window: Don't take focus when clicked on.
        NoInputs = 1 << 7,   // Platform Window: Make mouse pass through so we can drag this window while peaking behind it.
        NoRendererClear = 1 << 8,   // Platform Window: Renderer doesn't need to clear the framebuffer ahead (because we will fill it entirely).
        NoAutoMerge = 1 << 9,   // Platform Window: Avoid merging this window into another host window. This can only be set via ImGuiWindowClass viewport flags override (because we need to now ahead if we are going to create a viewport in the first place!).
        TopMost = 1 << 10,  // Platform Window: Display on top (for tooltips only).
        CanHostOtherWindows = 1 << 11,  // Viewport can host multiple imgui windows (secondary viewports are associated to a single window). // FIXME: In practice there's still probably code making the assumption that this is always and only on the MainViewport. Will fix once we add support for "no main viewport".

        // Output status flags (from Platform)
        IsMinimized = 1 << 12,  // Platform Window: Window is minimized, can skip render. When minimized we tend to avoid using the viewport pos/size for clipping window or testing if they are contained in the viewport.
        IsFocused = 1 << 13,  // Platform Window: Window is focused (last call to Platform_GetWindowFocus() returned true)
    };
    ENUM_CLASS_FLAGS(ImGuiViewportFlags);

    struct ImGuiViewport
    {
        ImGuiID             id;                     // Unique identifier for the viewport
        ImGuiViewportFlags  flags;                  // See ImGuiViewportFlags_
        Vec2              pos;                    // Main Area: Position of the viewport (Dear ImGui coordinates are the same as OS desktop/native coordinates)
        Vec2              size;                   // Main Area: Size of the viewport.
        Vec2              workPos;                // Work Area: Position of the viewport minus task bars, menus bars, status bars (>= Pos)
        Vec2              workSize;               // Work Area: Size of the viewport minus task bars, menu bars, status bars (<= Size)
        float               dpiScale;               // 1.0f = 96 DPI = No extra scale.
        ImGuiID             parentViewportId;       // (Advanced) 0: no parent. Instruct the platform backend to setup a parent/child relationship between platform windows.
        void* drawData;               // The ImDrawData corresponding to this viewport. Valid after Render() and until the next call to NewFrame().

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

        ImGuiViewport() { memset(this, 0, sizeof(*this)); }
        ~ImGuiViewport() { ASSERT(platformUserData == NULL && rendererUserData == NULL); }

        // Helpers
        Vec2              GetCenter() const { return Vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f); }
        Vec2              GetWorkCenter() const { return Vec2(workPos.x + workSize.x * 0.5f, workPos.y + workSize.y * 0.5f); }
    };


    
} // namespace CE

