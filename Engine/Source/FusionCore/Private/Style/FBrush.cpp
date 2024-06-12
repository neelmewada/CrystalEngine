#include "FusionCore.h"

namespace CE
{
	FBrush::FBrush()
		: brushStyle(FBrushStyle::None)
		, tintColor(Color::Clear())
		, texturePath(Name())
		, tiling(FBrushTiling::None)
	{
		
	}

	FBrush::FBrush(const Color& solidFill)
		: brushStyle(FBrushStyle::SolidFill)
		, fillColor(solidFill)
	{

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

	bool FBrush::operator==(const FBrush& rhs) const
	{
		if (brushStyle != rhs.brushStyle)
			return false;
		if (tiling != rhs.tiling)
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

		brushStyle = from.brushStyle;

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
			tiling = from.tiling;
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
