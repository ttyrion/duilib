#pragma once
#include <DirectXMath/Inc/DirectXMath.h>

#define SAFE_DELETE(p) { if(p) delete p; p = NULL;}
#define SAFE_NDELETE(p) { if(p) delete[] p; p = NULL;}

namespace DuiLib {
    typedef struct {
        UINT width = 0;
        UINT height = 0;
        RECT dest = { 0 };
        RECT source = { 0 };
        //九宫格绘制，控制图片source区域：四条边x或y方向拉伸、四角不拉伸、中间区域x和y方向同时拉伸
        RECT corner = { 0 };
        DWORD mask = 0;
        UINT fade = 0;
        bool alpha_blend = false;
        CDuiString sDrawString;  //bkimage
        CDuiString sImageName;   //bkimage file
        std::string r;
        std::string g;
        std::string b;
        std::string a;        

        bool empty() const {
            return width == 0 || height == 0 || r.empty() || g.empty() || b.empty() || a.empty();
        }

        void clear() {
            width = height = 0;
            dest.left = dest.top = dest.right = dest.bottom = 0;
            source.left = source.top = source.right = source.bottom = 0;
            corner.left = corner.top = corner.right = corner.bottom = 0;
            mask = 0;
            fade = 0;
            sDrawString.Empty();
            sImageName.Empty();
            r.clear();
            g.clear();
            b.clear();
            a.clear();
        }
    } ImageData;

    typedef struct {
        DirectX::XMFLOAT3    pos;           // vertex untransformed position
        DirectX::XMFLOAT4    color;
    } COLOR_VERTEX;

    typedef struct {
        DirectX::XMFLOAT3    pos;
        DirectX::XMFLOAT2    tex;          //texture coordinates
    } TEXTURE_VERTEX;

} // namespace DuiLib
