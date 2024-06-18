#pragma once
#include "Layout/LayoutTypes.h"

namespace CE
{
    ENUM()
    enum class FBrushStyle : u8
    {
	    None = 0,
        SolidFill,
        TexturePattern,
        LinearGradient
    };
    ENUM_CLASS(FBrushStyle);

    ENUM(Flags)
    enum class FBrushTiling : u8
    {
	    None = 0,
        TileX = BIT(0),
        TileY = BIT(1),
        TileXY = TileX | TileY
    };
    ENUM_CLASS_FLAGS(FBrushTiling);

    STRUCT()
    struct FUSIONCORE_API FBrush final
    {
        CE_STRUCT(FBrush)
    public:

        FBrush();

        FBrush(const Color& fillColor, FBrushStyle brushStyle = FBrushStyle::SolidFill);

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
        const Name& GetTexturePath() const { return texturePath; }

        const Vec2& GetBrushSize() const { return brushSize; }
        void SetBrushSize(Vec2 brushSize) { this->brushSize = brushSize; }

        HAlign GetHAlign() const { return hAlign; }
        VAlign GetVAlign() const { return vAlign; }

        void SetHAlign(HAlign hAlign) { this->hAlign = hAlign; }
        void SetVAlign(VAlign vAlign) { this->vAlign = vAlign; }

        bool operator==(const FBrush& rhs) const;

        bool operator!=(const FBrush& rhs) const
        {
            return !operator==(rhs);
        }

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
                Name texturePath;
            };
        };

        Vec2 brushSize = Vec2(0, 0);
        HAlign hAlign = HAlign::Auto;
        VAlign vAlign = VAlign::Auto;
        FBrushTiling tiling = FBrushTiling::None;
        FBrushStyle brushStyle = FBrushStyle::None;
    };
    
} // namespace CE

#include "FBrush.rtti.h"