#pragma once
#include <d3d11.h>
#include <sstream>
#include "D3DTypes.h"

#if defined(DEBUG) || defined(_DEBUG)
#define Direct3DFailedDebugMsgBox(hr, r, msg) if(FAILED(hr)) { \
    std::wstringstream ss; ss << msg; \
    ::MessageBox(NULL, ss.str().c_str(), L"", MB_OK); \
    return r; \
}
#else
#define Direct3DFailedDebugMsgBox(hr, r, msg) ""
#endif

#define Direct3DMsgBox(msg) \
    std::wstringstream ss; ss << msg; \
    ::MessageBox(NULL, ss.str().c_str(), L"", MB_OK); \

#define ReleaseCOMInterface(x) { if(x){ x->Release(); x = NULL; } }
#define MapScreenX(x,width) { (x-(float)width/2) / (width / 2) }
#define MapScreenY(y,height) { -(y-(float)height/2) / (height / 2) }

#define MapTextureXY(c,d) { c / (float)d }

//#define GetA(color) (color && 0xFF000000) >> 24
//#define GetR(color) (color && 0x00FF0000) >> 16
//#define GetG(color) (color && 0x0000FF00) >> 8
//#define GetB(color) (color && 0x000000FF)

namespace DuiLib {
    class DUILIB_API Direct3DRender
    {
    public:
        Direct3DRender();
        ~Direct3DRender();

        bool InitDirect3D(HWND render_window);
        void ReleaseDirect3D();

        void ResizeRender(const RECT& render_rect);
        void PresentScene();

        void DrawColor(const RECT& render_rect, DWORD dwcolor);


        void DrawBkColor(const RECT&rect, DWORD color);
        bool LoadImage(ImageData& image);
        bool DrawImage(const RECT& rcItem, const RECT& rcPaint, ImageData& image);
        void DrawStatusImage();
        void DrawText();
        void DrawBorder();

    private:
        bool IASetColorLayout();
        bool IASetTextureLayout();
        
        bool CreateTextureResource(const UINT width, const UINT height);
        bool UpdateTextureResource(const ImageData& image);
        bool SetLinearSamplerState();
        
        std::string LoadShader(const std::string& cso_file);
    private:
        UINT width_ = 0;
        UINT height_ = 0;
        UINT x4_msaa_uality_ = 0;

        ID3D11Device* d3d_device_ = NULL;
        ID3D11DeviceContext* d3d_immediate_context_ = NULL;
        IDXGISwapChain* d3d_swap_chain_ = NULL;

        ID3D11DepthStencilState* depth_stenci_state_ = NULL;
        ID3D11DepthStencilState* disable_depth_stenci_state_ = NULL;
        ID3D11Texture2D* d3d_depth_stencil_buffer_ = NULL;
        ID3D11RasterizerState* raster_state_ = NULL;

        ID3D11RenderTargetView* d3d_render_target_view_ = NULL;
        ID3D11DepthStencilView* d3d_depth_stencil_view_ = NULL;
        D3D11_VIEWPORT d3d_screen_viewport_;

        //控制着色器
        ID3D11SamplerState* sampler_state_ = NULL;
        //用四层文理资源分别访问图片的r,g,b,a 数据，相应地，着色器中也要定义四个纹理资源
        ID3D11Texture2D* texture_planes_[4] = { NULL };
        ID3D11ShaderResourceView* texture_resource_views_[4] = { NULL };
        ID3D11SamplerState* linear_sampler_state_ = NULL;
        UINT resource_width_ = 0;
        UINT resource_height_ = 0;
    };
} // namespace DuiLib
