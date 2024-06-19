#include "FusionCore.h"

#include "stb_image_write.h"

#undef DLL_EXPORT
#define FT_PUBLIC_FUNCTION_ATTRIBUTE

#include <ft2build.h>
#include "freetype/freetype.h"

namespace CE
{
    extern RawData GetRobotoFont();

    FFontManager::FFontManager()
    {
        
    }

    void FFontManager::Init()
    {
        FT_Init_FreeType(&ft);

        RawData robotoFontFile = GetRobotoFont();

        Array<CharRange> englishCharSets{};
        englishCharSets.Add(CharRange('a', 'z'));
        englishCharSets.Add(CharRange('A', 'Z'));
        englishCharSets.Add(CharRange('0', '9'));
        englishCharSets.Add(CharRange(32, 47));
        englishCharSets.Add(CharRange(58, 64));
        englishCharSets.Add(CharRange(91, 96));
        englishCharSets.Add(CharRange(123, 126));

        MemoryStream stream = MemoryStream(robotoFontFile.data, robotoFontFile.dataSize, Stream::Permissions::ReadOnly);

        RegisterFont("Roboto", englishCharSets, &stream);
    }

    void FFontManager::Shutdown()
    {
        for (auto& [fontName, atlas] : fontAtlases)
        {
	        if (atlas->face)
	        {
                FT_Done_Face(atlas->face);
                atlas->face = nullptr;
	        }
            if (atlas->faceItalic)
            {
                FT_Done_Face(atlas->faceItalic);
                atlas->faceItalic = nullptr;
            }
            atlas->ft = nullptr;

            atlas->Destroy();
        }

        FT_Done_FreeType(ft);
        ft = nullptr;

        fontAtlases.Clear();
    }

    bool FFontManager::RegisterFont(const Name& fontName, const Array<CharRange>& characterSets,
                                    MemoryStream* ttfFontFile)
    {
        if (!fontName.IsValid() || characterSets.IsEmpty() || ttfFontFile == nullptr ||
            !ttfFontFile->CanRead() || ttfFontFile->GetLength() == 0)
            return false;

        if (fontAtlases.KeyExists(fontName) && fontAtlases[fontName] != nullptr)
        {
            return true;
        }

        FFontAtlas* fontAtlas = CreateObject<FFontAtlas>(this, fontName.GetString());

        FT_Face face;

        if (FT_New_Memory_Face(ft, (const FT_Byte*)ttfFontFile->GetRawDataPtr(), ttfFontFile->GetLength(), 0, &face))
        {
            return false;
        }

        fontAtlas->ft = ft;
        fontAtlas->face = face;
        fontAtlas->faceItalic = nullptr;

        Array<u32> charSet{};
        charSet.Reserve(256);
        for (auto rangeInfo : characterSets)
        {
            if (rangeInfo.charCode != 0)
            {
                charSet.Add(rangeInfo.charCode);
            }
            else if (rangeInfo.range.min <= rangeInfo.range.max)
            {
                for (FT_ULong c = rangeInfo.range.min; c <= rangeInfo.range.max; c++)
                {
                    charSet.Add(c);
                }
            }
        }

        fontAtlas->Init(charSet, 14);

        return true;
    }

} // namespace CE
