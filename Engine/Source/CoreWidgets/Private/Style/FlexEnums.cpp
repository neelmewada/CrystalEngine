#include "CoreWidgets.h"

namespace CE::Widgets
{

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

	COREWIDGETS_API CAlign StringToAlign(const String& string)
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

	COREWIDGETS_API CJustify StringToJustify(const String& string)
	{
		return stringToJustifyMap[string];
	}

	static HashMap<String, CFlexDirection> stringToFlexDirectionMap{
		{ "column", CFlexDirection::Column },
		{ "column-reverse", CFlexDirection::ColumnReverse },
		{ "row", CFlexDirection::Row },
		{ "row-reverse", CFlexDirection::RowReverse },
	};

	COREWIDGETS_API CFlexDirection StringToFlexDirection(const String& string)
	{
		return stringToFlexDirectionMap[string];
	}

	static HashMap<String, CFlexWrap> stringToFlexWrapMap{
		{ "nowrap", CFlexWrap::NoWrap },
		{ "wrap", CFlexWrap::Wrap },
		{ "wrap-reverse", CFlexWrap::WrapReverse }
	};

	COREWIDGETS_API CFlexWrap StringToFlexWrap(const String& string)
	{
		return stringToFlexWrapMap[string];
	}

} // namespace CE::Widgets
