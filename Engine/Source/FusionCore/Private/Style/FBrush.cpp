#include "FusionCore.h"

namespace CE
{
	

	FBrush::FBrush()
		: tintColor(Color::Clear())
		, imageName(Name())
		, tiling(FBrushTiling::None)
		, brushStyle(FBrushStyle::None)
		, imageFit(FImageFit::Fill)
	{

	}

	FBrush::FBrush(const Color& fillColor, FBrushStyle brushStyle)
		: fillColor(fillColor)
		, brushStyle(brushStyle)
		, imageFit(FImageFit::Fill)
	{

	}

	FBrush::FBrush(const Name& imageName, const Color& tintColor)
		: tintColor(tintColor)
		, imageName(imageName)
		, tiling(FBrushTiling::None)
		, brushStyle(FBrushStyle::Image)
		, imageFit(FImageFit::Fill)
	{
		
	}

	FBrush::~FBrush()
	{
		switch (brushStyle)
		{
		case FBrushStyle::Image:
			imageName.~Name();
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
		case FBrushStyle::Image:
			return tintColor.a > 0 && imageName.IsValid();
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
		if (imageFit != rhs.imageFit)
			return false;
		if (brushPos != rhs.brushPos)
			return false;
		if (brushSize != rhs.brushSize)
			return false;

		switch (brushStyle)
		{
		case FBrushStyle::SolidFill:
			return fillColor == rhs.fillColor;
		case FBrushStyle::LinearGradient:
			break;
		case FBrushStyle::Image:
			return tintColor == rhs.tintColor && imageName == rhs.imageName;
		case FBrushStyle::None:
			break;
		}

		return true;
	}

	FBrush FBrush::WithTint(const Color& tintColor) const
	{
		FBrush result = *this;
		result.tintColor = tintColor;
		return result;
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
		    case FBrushStyle::Image:
				imageName.~Name();
			    break;
		    }
	    }

		brushSize = from.brushSize;
		imageFit = from.imageFit;
		brushPos = from.brushPos;
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
		case FBrushStyle::Image:
			tintColor = from.tintColor;
			imageName = from.imageName;
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
		move.brushPos = {};
		move.brushSize = {};
		move.imageFit = {};
		move.imageName.~Name();
    }

    
} // namespace CE
