#include "stdafx.h"
#include "FreeTypeFont.h"
#include <fstream>
#include FT_OUTLINE_H
#include FT_GLYPH_H

#define FT_POS_COEF  (1.0/64.0)

FT_Library DuiLib::FreeTypeFont::lib_ = nullptr;
UINT DuiLib::FreeTypeFont::ft_used_count_ = 0;
std::wstring DuiLib::FreeTypeFont::system_fonts_dir_;

namespace DuiLib {

    FreeTypeFont::FreeTypeFont(const std::wstring& font_name, const UINT font_size, const bool antialias)
    :font_size_(font_size), font_name_(font_name){

        ++ft_used_count_;
    }

    FreeTypeFont::~FreeTypeFont() {
        if (face_) {
            FT_Done_Face(face_);
            face_ = nullptr;
        }
        

        --ft_used_count_;
    }

    bool FreeTypeFont::Init() {
        if (!lib_) {
            FT_Error err = FT_Init_FreeType(&lib_);
            if (err) {
                return false;
            }
        }

        WCHAR buffer[MAX_PATH] = { 0 };
        if (!::GetWindowsDirectory(buffer, MAX_PATH)) {
            return false;
        }
        system_fonts_dir_ = buffer;
        system_fonts_dir_ += L"\\Fonts";

        return true;
    }

    void FreeTypeFont::UnInit() {
        if (lib_) {
            FT_Done_FreeType(lib_);
        }

        lib_ = nullptr;
    }

    bool FreeTypeFont::LoadFont() {
        std::wstring file = system_fonts_dir_ + L"\\msyh.ttc";

        if (!::PathFileExists(file.c_str())) {
            return false;
        }

        std::string font_file = CW2AEX<>(file.c_str(), CP_UTF8);
        //A font file may contain multiple faces
        if (face_) {
            FT_Done_Face(face_);
            face_ = nullptr;
        }
        FT_Error err = FT_New_Face(lib_, font_file.c_str(), 0, &face_);
        
        if (err == FT_Err_Unknown_File_Format) {
            //font file accessed successfully, but it has an unknown format
            return false;
        }
        else if (err) {
            //failed on other reasons
            return false;
        }

        //Actually by default, when a new face object is created, FreeType tries to select a Unicode charmap
        FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
        if (!face_->charmap) {
            FT_Done_Face(face_);
            face_ = nullptr;

            //the current font does not have a Unicode charmap
            return false;
        }

        //Here we should specify the size of glyph we want to generate with the face object.

        /*Note:
             1. The character widths and heights are specified in 1/64th of points
             2. A point is a physical distance, equaling 1/72th of an inch. Normally, it is not equivalent to a pixel.
             3. The device resolutions are expressed in dots-per-inch("dpi"). 
                Standard values are 72 or 96 dpi for display devices like the screen.
                The resolutions are used to compute the character pixel size from the character point size.
        */
        //FT_F26Dot6 char_width = font_size_ * 64;
        //FT_F26Dot6 char_height = char_width;
        //FT_UInt dpi = 96;
        //err = FT_Set_Char_Size(face_, char_width, char_height, dpi, dpi);
        //if (err) {
        //    //TODO: why?
        //    float ptSize_72 = (font_size_ * 72.0f) / dpi;
        //    float best_delta = 99999;
        //    float best_size = 0;
        //    for (int i = 0; i < face_->num_fixed_sizes; i++)
        //    {
        //        float size = face_->available_sizes[i].size * float(FT_POS_COEF);
        //        float delta = fabs(size - ptSize_72);
        //        if (delta < best_delta)
        //        {
        //            best_delta = delta;
        //            best_size = size;
        //        }
        //    }

        //    if ((best_size <= 0) || FT_Set_Char_Size(face_, 0, FT_F26Dot6(best_size * 64), 0, 0)) {
        //        //still error
        //        return false;
        //    }
        //}


        //We specify the (integer) pixel sizes ourselves, but not using the FT_Set_Char_Size
        //with a zero font width set, 
        //the final width of a glyph would be caculated dynamically according to the font height
        FT_Set_Pixel_Sizes(face_, 0, font_size_);

        //if character glyph images can be rendered for all character pixel sizes
        //if (face_->face_flags & FT_FACE_FLAG_SCALABLE) {
        //    float y_scale = face_->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f / 65536.0f);
        //    ascender_ = face_->ascender * y_scale;
        //    descender_ = face_->descender * y_scale;
        //    font_height_ = face_->height * y_scale;
        //}
        //else
        //{
        //    ascender_ = face_->size->metrics.ascender * float(FT_POS_COEF);
        //    descender_ = face_->size->metrics.descender * float(FT_POS_COEF);
        //    font_height_ = face_->size->metrics.height * float(FT_POS_COEF);
        //}


        return true;
    }

    bool FreeTypeFont::GetTextData(UINT char_utf32_code, TextData& data) {
        if (!face_) {
            return false;
        }

        FT_UInt glyph_index = FT_Get_Char_Index(face_, char_utf32_code);
        FT_Error err = FT_Load_Glyph(face_, glyph_index, FT_LOAD_DEFAULT);
        if (err) {
            return false;
        }

        if (face_->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
            err = FT_Render_Glyph(face_->glyph, FT_RENDER_MODE_NORMAL);
            if (err || face_->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
                return false;
            }
        }

        //FT_Glyph glyph;
        //err = FT_Get_Glyph(face_->glyph, &glyph);
        //if (err) {
        //    return false;
        //}

        //auto ret = [&glyph]( bool val) {
        //    FT_Done_Glyph(glyph);
        //    return val;
        //};

        data.bitmap.width = face_->glyph->bitmap.width;
        data.bitmap.height = face_->glyph->bitmap.rows;
        data.metrics.width = face_->glyph->metrics.width / 64;
        data.metrics.height = face_->glyph->metrics.height / 64;
        data.metrics.bearingX = face_->glyph->metrics.horiBearingX / 64;
        data.metrics.bearingY = face_->glyph->metrics.horiBearingY / 64;
        data.metrics.advance = face_->glyph->metrics.horiAdvance / 64;

        switch (face_->glyph->bitmap.pixel_mode) {
        case FT_PIXEL_MODE_BGRA: {

        }
                                 break;
        case FT_PIXEL_MODE_GRAY: {
            data.bitmap.format = IMAGE_FORMAT_GRAY;            
            //image.source.right = image.bitmap.width;
            //image.source.bottom = image.bitmap.height;
            data.bitmap.buffer.append((char*)face_->glyph->bitmap.buffer, data.bitmap.width * data.bitmap.height);
        }
                                 break;
        default:
            return false;
        }

        return true;
    }

    //bool FreeTypeFont::GetTextBitmap(WCHAR c, ImageData& image) {
    //    if (!face_) {
    //        return false;
    //    }

    //    //FT_LOAD_RENDER does the same work with FT_Render_Glyph(...FT_RENDER_MODE_NORMAL)
    //    FT_Error err = FT_Load_Char(face_, c, FT_LOAD_RENDER);
    //    if (err) {
    //        return false;
    //    }

    //    switch (face_->glyph->bitmap.pixel_mode) {
    //    case FT_PIXEL_MODE_BGRA: {
    //            
    //        }
    //        break;
    //    case FT_PIXEL_MODE_GRAY: {
    //            image.format = IMAGE_FORMAT_GRAY;
    //            image.width = face_->glyph->bitmap.width;
    //            image.height = face_->glyph->bitmap.rows;
    //            image.source.right = image.width;
    //            image.source.bottom = image.height;
    //            image.buffer.append((char*)face_->glyph->bitmap.buffer, image.width * image.height);
    //        }
    //        break;
    //    default:
    //        return false;
    //    }
    //}
}
