#pragma once
#include "Layout/LayoutTypes.h"

namespace CE
{
    ENUM()
    enum class FBrushStyle : u8
    {
	    None = 0,
        SolidFill,
        Image,
        LinearGradient
    };
    ENUM_CLASS(FBrushStyle);

    ENUM()
    enum class FBrushTiling : u8
    {
	    None = 0,
        TileX,
        TileY,
        TileXY
    };
    ENUM_CLASS(FBrushTiling);

    ENUM()
    enum class FImageFit : u8
    {
        None = 0,
	    Fill,
        Contain,
        Cover
    };

    STRUCT()
    struct FUSIONCORE_API FBrush final
    {
        CE_STRUCT(FBrush)
    public:

        FBrush();

        FBrush(const Color& fillColor, FBrushStyle brushStyle = FBrushStyle::SolidFill);

        FBrush(const Name& imageName, const Color& tintColor = Color::White());

        ~FBrush();

        FBrush(const FBrush& copy);
        FBrush& operator=(const FBrush& copy);

        FBrush(FBrush&& move) noexcept;
        FBrush& operator=(FBrush&& move) noexcept;

        bool IsValidBrush();

        FBrushStyle GetBrushStyle() const { return brushStyle; }
        FBrushTiling GetBrushTiling() const { return tiling; }

        void SetBrushStyle(FBrushStyle brushStyle) { this->brushStyle = brushStyle; }
        void SetBrushTiling(FBrushTiling tiling) { this->tiling = tiling; }

        const Color& GetFillColor() const { return fillColor; }

        const Color& GetTintColor() const { return tintColor; }
        const Name& GetImageName() const { return imageName; }

        FImageFit GetImageFit() const { return imageFit; }
        void SetImageFit(FImageFit imageFit) { this->imageFit = imageFit; }

        const Vec2& GetBrushSize() const { return brushSize; }
        void SetBrushSize(Vec2 brushSize) { this->brushSize = brushSize; }

        const Vec2& GetBrushPosition() const { return brushPos; }
        void SetBrushPosition(Vec2 brushPos) { this->brushPos = brushPos; }

        bool operator==(const FBrush& rhs) const;

        bool operator!=(const FBrush& rhs) const
        {
            return !operator==(rhs);
        }

        FBrush WithTint(const Color& tintColor) const;

    private:

        void CopyFrom(const FBrush& from);
        void MoveFrom(FBrush& move);

        union 
        {
            // Solid Fill
            struct
            {
                Color fillColor;
            };

            // Texture
            struct
            {
                Color tintColor;
                Name imageName;
            };
        };

        Vec2 brushSize = Vec2(-1, -1); // -1 means auto size
        Vec2 brushPos = Vec2(0.5f, 0.5f); // 50% means centered
        FBrushTiling tiling = FBrushTiling::None;
        FBrushStyle brushStyle = FBrushStyle::None;
        FImageFit imageFit = FImageFit::None;

        friend class FPainter;
        friend class FusionRenderer;
    };
    
} // namespace CE

#include "FBrush.rtti.h"