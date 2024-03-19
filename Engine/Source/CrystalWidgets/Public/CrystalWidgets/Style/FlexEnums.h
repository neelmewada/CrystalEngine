#pragma once

namespace CE::Widgets
{
	ENUM()
	enum class CDisplay
	{
		Flex = 0,
		None
	};
	ENUM_CLASS_FLAGS(CDisplay);

	CRYSTALWIDGETS_API CDisplay StringToDisplay(const String& string);

	ENUM()
	enum class CPosition
	{
		Static = 0,
		Relative,
		Absolute,
	};
	ENUM_CLASS_FLAGS(CPosition);

	CRYSTALWIDGETS_API CPosition StringToPosition(const String& string);
    
	ENUM()
	enum class CAlign
	{
		Auto,
		FlexStart,
		Center,
		FlexEnd,
		Stretch,
		Baseline,
		SpaceBetween,
		SpaceAround
	};
	ENUM_CLASS_FLAGS(CAlign);

	CRYSTALWIDGETS_API CAlign StringToAlign(const String& string);

	ENUM()
	enum class CJustify
	{
		FlexStart,
		Center,
		FlexEnd,
		SpaceBetween,
		SpaceAround,
		SpaceEvenly
	};
	ENUM_CLASS_FLAGS(CJustify);

	CRYSTALWIDGETS_API CJustify StringToJustify(const String& string);

	ENUM()
	enum class CFlexDirection
	{
		Column,
		ColumnReverse,
		Row,
		RowReverse
	};
	ENUM_CLASS_FLAGS(CFlexDirection);

	CRYSTALWIDGETS_API CFlexDirection StringToFlexDirection(const String& string);

	ENUM()
	enum class CFlexWrap
	{
		NoWrap,
		Wrap,
		WrapReverse
	};
	ENUM_CLASS_FLAGS(CFlexWrap);

	CRYSTALWIDGETS_API CFlexWrap StringToFlexWrap(const String& string);

} // namespace CE::Widgets


#include "FlexEnums.rtti.h"
