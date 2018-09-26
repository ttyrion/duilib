#pragma once

/*
    Core render with Direct3D 11 added by tyrion.
*/

#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")

#include <d3d11.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <dwrite.h>
#include <sstream>
#include <vector>
#include "D3DTypes.h"
#include "UITypedef.h"

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
        void BeginDraw();
        void EndDraw();

        bool FillColor(const RECT& rect, DWORD color);
        bool DrawImage(const RECT& item_rect, const RECT& paint_rect, ImageData& image, const DWORD bkcolor);
        bool DrawVideoFrame(const RECT& item_rect, const RECT& paint_rect, const VideoFrame& frame);
        void DrawText(const RECT& text_rect, const CDuiString& text, const TFontInfo& font_info, DWORD color, UINT text_style);
        void DrawText2D(const RECT& text_rect, const CDuiString& text, const TFontInfo& font_info, DWORD color, UINT text_style);
        bool DrawBorder(const RECT& item_rect, const UINT border_size, DWORD color);

        static bool LoadImage(ImageData& image);
        static std::string Direct3DRender::GetShaderFileName(DuiLib::SHADER_TYPE type);
        static std::string LoadShader(DuiLib::SHADER_TYPE type);
        static void SaveShader();

    private:
        bool CreateTextRenderTarget(const UINT width, const UINT height);
        //initialize Direct2D, Direct3D 10_1, and DirectWrite
        bool Init2DTextRender(IDXGIAdapter1 *adapter, const UINT width, const UINT height);
        void Init2DScreenTexture();
        bool IASetColorLayout();
        bool IASetTextureLayout(SHADER_TYPE vertex_shader_type, SHADER_TYPE pixel_shader_type);
        bool IASetRGBATextureLayout();
        bool IASetGrayTextureLayout();
        
        bool CreateTextureResource(const UINT width, const UINT height, IMAGE_FORMAT format);
        bool UpdateTextureResource(const DuiBitmap& bitmap);
        bool CreateFrameTextureResource(const VideoFrame& frame);
        bool UpdateFrameTextureResource(const VideoFrame& frame);
        bool SetLinearSamplerState();
        
        bool DrawColorVertex(const std::vector<COLOR_VERTEX>& vertice, const std::vector<WORD>& indice, const D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        bool DrawLine(const POINT& begin, const POINT& end, DWORD dwcolor);
        bool DrawRect(const RECT& rect, DWORD color);

    private:
        static std::vector<std::string> shaders_;
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
        //ID3D11SamplerState* sampler_state_ = NULL;
        ID3D11BlendState* text_blend_state_ = NULL;

        //用三层文理资源分别访问一帧视频的yuv数据
        UINT frame_width_ = 0;
        UINT frame_height_ = 0;
        ID3D11Texture2D* frame_texture_planes_[3] = { NULL };
        ID3D11ShaderResourceView* frame_texture_resource_views_[3] = { NULL };
        //改用一个纹理资源，处理多种图片格式
        ID3D11Texture2D* texture_planes_ = NULL;
        ID3D11ShaderResourceView* texture_resource_views_ = NULL;

        ID3D11SamplerState* linear_sampler_state_ = NULL;
        UINT resource_width_ = 0;
        UINT resource_height_ = 0;


        //Render Text        
        ID3D10Device1* d3d10_1_device_;
        IDXGIKeyedMutex* keyed_mutex_11_;
        IDXGIKeyedMutex* keyed_mutex_10_;

        // They are Device-Dependent Resources(bound to a rendering device when they are created 
        // and must be released and recreated if the device becomes invalid.)
        ID2D1RenderTarget* text_render_target_;
        ID2D1SolidColorBrush* text_brush_;

        ID3D11Texture2D* shared_texture_;
        ID3D11ShaderResourceView* shared_texture_resource_view_;
        ID3D11Buffer* text_plane_vbuffer_;
        ID3D11Buffer* text_plane_ibuffer_;        
        IDWriteFactory* dwrite_factory_;
        //IDWriteTextFormat* text_format;
        std::map<D2DFontKey, IDWriteTextFormat*, D2DFontLess> text_formats_;
    };
} // namespace DuiLib
