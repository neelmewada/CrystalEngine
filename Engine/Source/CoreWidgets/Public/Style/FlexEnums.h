#pragma once

namespace CE::Widgets
{
    
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

	COREWIDGETS_API CAlign StringToAlign(const String& string);

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

	COREWIDGETS_API CJustify StringToJustify(const String& string);

	ENUM()
	enum class CFlexDirection
	{
		Column,
		ColumnReverse,
		Row,
		RowReverse
	};
	ENUM_CLASS_FLAGS(CFlexDirection);

	COREWIDGETS_API CFlexDirection StringToFlexDirection(const String& string);

	ENUM()
	enum class CFlexWrap
	{
		NoWrap,
		Wrap,
		WrapReverse
	};
	ENUM_CLASS_FLAGS(CFlexWrap);

	COREWIDGETS_API CFlexWrap StringToFlexWrap(const String& string);

} // namespace CE::Widgets


#include "FlexEnums.rtti.h"
