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

	ENUM()
	enum class CFlexDirection
	{
		Column,
		ColumnReverse,
		Row,
		RowReverse
	};
	ENUM_CLASS_FLAGS(CFlexDirection);

	ENUM()
	enum class CFlexWrap
	{
		NoWrap,
		Wrap,
		WrapReverse
	};
	ENUM_CLASS_FLAGS(CFlexWrap);

} // namespace CE::Widgets


#include "FlexEnums.rtti.h"
