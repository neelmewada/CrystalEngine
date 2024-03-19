#include "CrystalWidgets.h"

namespace CE::Widgets
{

	static HashMap<String, CDisplay> stringToDisplayMap{
		{ "none", CDisplay::None },
		{ "flex", CDisplay::Flex },
	};

	CRYSTALWIDGETS_API CDisplay StringToDisplay(const String& string)
	{
		return stringToDisplayMap[string];
	}

	static HashMap<String, CPosition> stringToPositionMap{
		{ "auto", CPosition::Static },
		{ "static", CPosition::Static },
		{ "absolute", CPosition::Absolute },
		{ "relative", CPosition::Relative }
	};

    CRYSTALWIDGETS_API CPosition StringToPosition(const String& string)
	{
		return stringToPositionMap[string];
	}

	static HashMap<String, CAlign> stringToAlignMap{
		{ "auto", CAlign::Auto },
		{ "baseline", CAlign::Baseline },
		{ "center", CAlign::Center },
		{ "flex-start", CAlign::FlexStart },
		{ "flex-end", CAlign::FlexEnd },
		{ "space-between", CAlign::SpaceBetween },
		{ "space-around", CAlign::SpaceAround },
		{ "stretch", CAlign::Stretch },
	};

    CRYSTALWIDGETS_API CAlign StringToAlign(const String& string)
	{
		return stringToAlignMap[string];
	}

	static HashMap<String, CJustify> stringToJustifyMap{
		{ "center", CJustify::Center },
		{ "flex-start", CJustify::FlexStart },
		{ "flex-end", CJustify::FlexEnd },
		{ "space-around", CJustify::SpaceAround },
		{ "space-between", CJustify::SpaceBetween },
		{ "space-evenly", CJustify::SpaceEvenly },
	};

	CRYSTALWIDGETS_API CJustify StringToJustify(const String& string)
	{
		return stringToJustifyMap[string];
	}

	static HashMap<String, CFlexDirection> stringToFlexDirectionMap{
		{ "column", CFlexDirection::Column },
		{ "column-reverse", CFlexDirection::ColumnReverse },
		{ "row", CFlexDirection::Row },
		{ "row-reverse", CFlexDirection::RowReverse },
	};

	CRYSTALWIDGETS_API CFlexDirection StringToFlexDirection(const String& string)
	{
		return stringToFlexDirectionMap[string];
	}

	static HashMap<String, CFlexWrap> stringToFlexWrapMap{
		{ "nowrap", CFlexWrap::NoWrap },
		{ "wrap", CFlexWrap::Wrap },
		{ "wrap-reverse", CFlexWrap::WrapReverse }
	};

	CRYSTALWIDGETS_API CFlexWrap StringToFlexWrap(const String& string)
	{
		return stringToFlexWrapMap[string];
	}

} // namespace CE::Widgets
