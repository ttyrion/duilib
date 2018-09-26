#pragma once
#include <string>
#include <DirectXMath/Inc/DirectXMath.h>

#define SAFE_DELETE(p) { if(p) delete p; p = NULL;}
#define SAFE_NDELETE(p) { if(p) delete[] p; p = NULL;}

namespace DuiLib {
    enum IMAGE_FORMAT {
        IMAGE_FORMAT_None = 0,
        IMAGE_FORMAT_GRAY = 1,
        IMAGE_FORMAT_RGBA = 4   
    };

    //枚举类型的值不能随意更改
    enum SHADER_TYPE {
        SHADER_TYPE_Color_Pixel = 0,
        SHADER_TYPE_Color_Vertex,
        SHADER_TYPE_RGBA_Pixel,
        SHADER_TYPE_RGBA_Vertex,
        SHADER_TYPE_VIDEO_Pixel,
        SHADER_TYPE_VIDEO_Vertex,
        SHADER_TYPE_Max
    };

    struct DuiBitmap {
        UINT width = 0;
        UINT height = 0;
        IMAGE_FORMAT format = IMAGE_FORMAT_None;
        std::string buffer;

        bool empty() const {
            return width == 0 || height == 0 || buffer.empty();
        }

        void clear() {
            width = height = 0;
            format = IMAGE_FORMAT_RGBA;
            buffer.clear();
        }
    };

    struct ImageData {
        bool alpha_blend = false;
        UINT fade = 255;
        DWORD mask = 0;
        RECT dest = { 0 };
        RECT source = { 0 };
        //九宫格绘制，控制图片source区域：四条边x或y方向拉伸、四角不拉伸、中间区域x和y方向同时拉伸
        RECT corner = { 0 };
        CDuiString sDrawString;  //bkimage config
        CDuiString sImageName;   //bkimage file    
        DuiBitmap bitmap;

        bool empty() const {
            return bitmap.empty();
        }

        void clear() {
            bitmap.clear();
            fade = 255;
            mask = 0;
            dest.left = dest.top = dest.right = dest.bottom = 0;
            source.left = source.top = source.right = source.bottom = 0;
            corner.left = corner.top = corner.right = corner.bottom = 0;         
            sDrawString.Empty();
            sImageName.Empty();            
        }
    };

    struct TextMetrics{
        signed long width = 0;
        signed long height = 0;
        signed long bearingX = 0;
        signed long bearingY = 0;
        signed long advance = 0;
    };

    struct D2DFontKey {
        D2DFontKey(const std::wstring& name, const UINT font_size, bool bold, bool underline, bool italic) {
            this->name = name;
            this->font_size = font_size;
            this->bold = bold;
            this->underline = underline;
            this->italic = italic;
        };

        std::wstring name;
        UINT font_size = 0;
        bool bold;
        bool underline;
        bool italic;
    };

    class D2DFontLess {
    public:
        bool operator() (const D2DFontKey& lhs, const D2DFontKey& rhs) const {
            return std::tie(lhs.name, lhs.font_size, lhs.bold, lhs.underline, lhs.italic)
                 < std::tie(rhs.name, rhs.font_size, rhs.bold, rhs.underline, rhs.italic);
        }
    };

    struct TextData {
        TextMetrics metrics;
        DuiBitmap bitmap;
    };

    struct COLOR_VERTEX {
        DirectX::XMFLOAT3    pos;           // vertex untransformed position
        DirectX::XMFLOAT4    color;
    };

    struct TEXTURE_VERTEX {
        DirectX::XMFLOAT3    pos;
        DirectX::XMFLOAT2    tex;          //texture coordinates
    };

} // namespace DuiLib
