#pragma once
#include <ft2build.h>
/*
    从FreeType 2.1.6开始，旧式的头文件包含模式将不会再被支持也就是说，下面的头文件包含方式会出错：
    #include <freetype/freetype.h>
    #include <freetype/ftglyph.h>
*/
#include FT_FREETYPE_H


namespace DuiLib {
    /*
        class FreeTypeFont is used to manage TrueType font.
    */
    class DUILIB_API FreeTypeFont {
    public:
        //font_name should not include the suffix such as ".ttf"
        FreeTypeFont(const std::wstring& font_name, const UINT font_size, const bool antialias);
        ~FreeTypeFont();

        static bool Init();
        static void UnInit();

        bool LoadFont();
        bool GetTextData(UINT char_utf32_code, TextData& data);
        //bool GetTextBitmap(WCHAR c, ImageData& image);

    private:
        static std::wstring system_fonts_dir_;
        static FT_Library lib_;
        FT_Face face_ = nullptr;
        static UINT ft_used_count_;
        static std::map<std::wstring, FreeTypeFont> font_map_;

        std::wstring font_name_;
        UINT font_size_ = 0;

        ////font layout
        //float ascender_ = 0.0f;
        //float descender_ = 0.0f;
        //float font_height_ = 0.0f;
    };
} // namespace DuiLib