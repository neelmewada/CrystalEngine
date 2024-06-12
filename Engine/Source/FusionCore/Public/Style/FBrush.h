#pragma once

namespace CE
{
    ENUM()
    enum class FBrushStyle : u8
    {
	    None = 0,
        SolidFill,
        LinearGradient,
        TexturePattern
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
    struct FUSIONCORE_API FBrush
    {
        CE_STRUCT(FBrush)
    public:

        FBrush();

        FBrush(const Color& solidFill);

        FBrush(const FBrush& copy);
        FBrush& operator=(const FBrush& copy);

        FBrush(FBrush&& move) noexcept;
        FBrush& operator=(FBrush&& move) noexcept;

        FBrushStyle GetBrushStyle() const { return brushStyle; }
        FBrushTiling GetBrushTiling() const { return tiling; }

        const Color& GetFillColor() const { return fillColor; }

        const Color& GetTintColor() const { return tintColor; }
        const Name& GetTexturePath() const { return texturePath; }

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

        FBrushTiling tiling = FBrushTiling::None;
        FBrushStyle brushStyle = FBrushStyle::None;
    };
    
} // namespace CE

#include "FBrush.rtti.h"