#pragma once
#include <d3d11.h>
#include <sstream>
#include <vector>
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

#define GETA(color) (int)((color & 0xFF000000) >> 24)
#define GETR(color) (int)((color & 0x00FF0000) >> 16)
#define GETG(color) (int)((color & 0x0000FF00) >> 8)
#define GETB(color) (int)(color & 0x000000FF)

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

        bool FillColor(const RECT& rect, DWORD color);
        bool DrawImage(const RECT& item_rect, const RECT& paint_rect, ImageData& image);
        void DrawStatusImage();
        void DrawText(const RECT& text_rect, const CDuiString& text, DWORD color);
        bool DrawBorder(const RECT& item_rect, const UINT border_size, DWORD color);

    private:
        bool IASetColorLayout();
        bool IASetTextureLayout(const std::string& vertex_shader_file, const std::string& pixel_shader_file);
        bool IASetRGBATextureLayout();
        bool IASetGrayTextureLayout();
        
        bool CreateTextureResource(const UINT width, const UINT height, IMAGE_FORMAT format);
        bool UpdateTextureResource(const ImageData& image);
        bool SetLinearSamplerState();

        bool LoadImage(ImageData& image);
        bool DrawColorVertex(const std::vector<COLOR_VERTEX>& vertice, const std::vector<WORD>& indice, const D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        bool DrawLine(const POINT& begin, const POINT& end, DWORD dwcolor);
        bool DrawRect(const RECT& rect, DWORD color);
        
        std::string LoadShader(const std::string& cso_file);
    private:
        bool initialized_ = false;
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
        //ID3D11Texture2D* texture_planes_[4] = { NULL };
        //ID3D11ShaderResourceView* texture_resource_views_[4] = { NULL };
        //改用一个纹理资源，处理多种图片格式
        ID3D11Texture2D* texture_planes_ = NULL;
        ID3D11ShaderResourceView* texture_resource_views_ = NULL;

        ID3D11SamplerState* linear_sampler_state_ = NULL;
        UINT resource_width_ = 0;
        UINT resource_height_ = 0;
    };
} // namespace DuiLib
