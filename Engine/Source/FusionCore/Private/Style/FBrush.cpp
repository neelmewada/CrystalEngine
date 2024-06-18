#include "FusionCore.h"

namespace CE
{
	FBrush::FBrush()
		: tintColor(Color::Clear())
		, texturePath(Name())
		, tiling(FBrushTiling::None)
		, brushStyle(FBrushStyle::None)
	{
		
	}

	FBrush::FBrush(const Color& fillColor, FBrushStyle brushStyle)
		: fillColor(fillColor)
		, brushStyle(brushStyle)
	{

	}

	FBrush::~FBrush()
	{
		switch (brushStyle)
		{
		case FBrushStyle::TexturePattern:
			texturePath.~Name();
			break;
		default:
			break;
		}
	}

	FBrush::FBrush(const FBrush& copy)
	{
		CopyFrom(copy);
	}

	FBrush& FBrush::operator=(const FBrush& copy)
	{
		CopyFrom(copy);
		return *this;
	}

	FBrush::FBrush(FBrush&& move) noexcept
	{
		MoveFrom(move);
	}

	FBrush& FBrush::operator=(FBrush&& move) noexcept
	{
		MoveFrom(move);
		return *this;
	}

	bool FBrush::IsValidBrush()
	{
		switch (brushStyle)
		{
		case FBrushStyle::None:
			return false;
		case FBrushStyle::SolidFill:
			return fillColor.a > 0;
		case FBrushStyle::TexturePattern:
			return tintColor.a > 0 && texturePath.IsValid();
		case FBrushStyle::LinearGradient:
			break;
		}

		return true;
	}

	bool FBrush::operator==(const FBrush& rhs) const
	{
		if (brushStyle != rhs.brushStyle)
			return false;
		if (tiling != rhs.tiling)
			return false;
		if (hAlign != rhs.hAlign)
			return false;
		if (vAlign != rhs.vAlign)
			return false;
		if (brushSize != rhs.brushSize)
			return false;

		switch (brushStyle)
		{
		case FBrushStyle::SolidFill:
			return fillColor == rhs.fillColor;
		case FBrushStyle::LinearGradient:
			break;
		case FBrushStyle::TexturePattern:
			return tintColor == rhs.tintColor && texturePath == rhs.texturePath;
		case FBrushStyle::None:
			break;
		}

		return true;
	}

	void FBrush::CopyFrom(const FBrush& from)
    {
	    if (brushStyle != from.brushStyle)
	    {
		    switch (brushStyle)
		    {
		    case FBrushStyle::None:
			    break;
		    case FBrushStyle::SolidFill:
			    break;
		    case FBrushStyle::LinearGradient:
			    break;
		    case FBrushStyle::TexturePattern:
				texturePath.~Name();
			    break;
		    }
	    }

		brushSize = from.brushSize;
		hAlign = from.hAlign;
		vAlign = from.vAlign;
		brushStyle = from.brushStyle;
		tiling = from.tiling;

	    switch (brushStyle)
	    {
		case FBrushStyle::None:
			break;
		case FBrushStyle::SolidFill:
			fillColor = from.fillColor;
			break;
		case FBrushStyle::LinearGradient:
			break;
		case FBrushStyle::TexturePattern:
			tintColor = from.tintColor;
			texturePath = from.texturePath;
			break;
	    }
    }

    void FBrush::MoveFrom(FBrush& move)
    {
		CopyFrom(move);

		move.brushStyle = FBrushStyle::None;
		move.fillColor = {};
		move.tintColor = {};
		move.tiling = {};
		move.texturePath.~Name();
    }

    
} // namespace CE
