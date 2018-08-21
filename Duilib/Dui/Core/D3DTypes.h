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

    struct D2DFont {
        D2DFont(const std::wstring& name, const UINT font_size) {
            this->name = name;
            this->font_size = font_size;
        };

        std::wstring name;
        UINT font_size = 0;
    };

    class D2DFontLess {
    public:
        bool operator() (const D2DFont& lhs, const D2DFont& rhs) const {
            return lhs.name < rhs.name && lhs.font_size < rhs.font_size;
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
