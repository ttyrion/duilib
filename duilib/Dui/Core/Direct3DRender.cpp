#include "stdafx.h"
#include "Direct3DRender.h"
#include <fstream>
#include <vector>
#include "D3DTypes.h"
#include "Direct3DImage.h"
#include "FreeTypeFont.h"

using namespace DirectX;

namespace DuiLib {

    Direct3DRender::Direct3DRender() {

    }


    Direct3DRender::~Direct3DRender() {
        for (auto& plane : texture_planes_) {
            ReleaseCOMInterface(plane);
        }

        for (auto& view : texture_resource_views_) {
            ReleaseCOMInterface(view);
        }

        ReleaseCOMInterface(d3d_swap_chain_);
        ReleaseCOMInterface(d3d_immediate_context_);
        ReleaseCOMInterface(d3d_device_);
    }

    bool Direct3DRender::InitDirect3D(HWND render_window) {
        if (!render_window) {
            return false;
        }

        UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        //激活调试层。当指定调试标志值后，Direct3D会向VC++的输出窗口发送调试信息
        create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        RECT rect;
        ::GetWindowRect(render_window, &rect);
        width_ = rect.right - rect.left;
        height_ = rect.bottom - rect.top;

        D3D_FEATURE_LEVEL feature_level;
        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = width_;
        sd.BufferDesc.Height = height_;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = render_window;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, create_device_flags, 0, 0, D3D11_SDK_VERSION, &sd, &d3d_swap_chain_, &d3d_device_, &feature_level, &d3d_immediate_context_);
        Direct3DFailedDebugMsgBox(hr, false, L"D3D11CreateDeviceAndSwapChain Failed.");

        if (feature_level != D3D_FEATURE_LEVEL_11_0) {
            Direct3DMsgBox(L"Feature Level 11 unsupported.");
            return false;
        }

        hr = d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &x4_msaa_uality_);
        Direct3DFailedDebugMsgBox(hr, false, L"CheckMultisampleQualityLevels Failed.");

        assert(x4_msaa_uality_ > 0);

        D3D11_DEPTH_STENCIL_DESC depth_stenci_desc;
        ZeroMemory(&depth_stenci_desc, sizeof(depth_stenci_desc));
        // Set up the description of the stencil state.
        depth_stenci_desc.DepthEnable = true;
        depth_stenci_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stenci_desc.DepthFunc = D3D11_COMPARISON_LESS;
        depth_stenci_desc.StencilEnable = true;
        depth_stenci_desc.StencilReadMask = 0xFF;
        depth_stenci_desc.StencilWriteMask = 0xFF;
        // Stencil operations if pixel is front-facing.
        depth_stenci_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depth_stenci_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depth_stenci_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depth_stenci_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        // Stencil operations if pixel is back-facing.
        depth_stenci_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depth_stenci_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depth_stenci_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depth_stenci_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        hr = d3d_device_->CreateDepthStencilState(&depth_stenci_desc, &depth_stenci_state_);
        Direct3DFailedDebugMsgBox(hr, false, L"create depth stencil state failed.");

        D3D11_DEPTH_STENCIL_DESC disable_depth_stenci_desc;
        ZeroMemory(&disable_depth_stenci_desc, sizeof(disable_depth_stenci_desc));
        // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
        // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
        disable_depth_stenci_desc.DepthEnable = false;
        disable_depth_stenci_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        disable_depth_stenci_desc.DepthFunc = D3D11_COMPARISON_LESS;
        disable_depth_stenci_desc.StencilEnable = true;
        disable_depth_stenci_desc.StencilReadMask = 0xFF;
        disable_depth_stenci_desc.StencilWriteMask = 0xFF;
        disable_depth_stenci_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        disable_depth_stenci_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        disable_depth_stenci_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        disable_depth_stenci_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        disable_depth_stenci_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        disable_depth_stenci_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        disable_depth_stenci_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        disable_depth_stenci_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        hr = d3d_device_->CreateDepthStencilState(&disable_depth_stenci_desc, &disable_depth_stenci_state_);
        Direct3DFailedDebugMsgBox(hr, false, L"create disable depth stencil state failed.");

        //diable the depth buffer
        d3d_immediate_context_->OMSetDepthStencilState(disable_depth_stenci_state_, 1);

        D3D11_RASTERIZER_DESC raster_desc;
        //Setup the raster description which will determine how and what polygons will be drawn.
        raster_desc.AntialiasedLineEnable = false;
        raster_desc.CullMode = D3D11_CULL_NONE;
        raster_desc.DepthBias = 0;
        raster_desc.DepthBiasClamp = 0.0f;
        raster_desc.DepthClipEnable = true;
        raster_desc.FillMode = D3D11_FILL_SOLID;
        raster_desc.FrontCounterClockwise = false;
        raster_desc.MultisampleEnable = false;
        raster_desc.ScissorEnable = false;
        raster_desc.SlopeScaledDepthBias = 0.0f;
        // Create the rasterizer state from the description we just filled out.
        hr = d3d_device_->CreateRasterizerState(&raster_desc, &raster_state_);
        Direct3DFailedDebugMsgBox(hr, false, L"create raster state failed.");
        //Now set the rasterizer state.
        d3d_immediate_context_->RSSetState(raster_state_);

        if (!SetLinearSamplerState()) {
            false;
        }

        if (!FreeTypeFont::Init()) {
            return false;
        }         

        initialized_ = true;
        return true;
    }

    void Direct3DRender::ReleaseDirect3D() {
        FreeTypeFont::UnInit();
        ReleaseCOMInterface(d3d_swap_chain_);
        ReleaseCOMInterface(d3d_immediate_context_);
        ReleaseCOMInterface(d3d_device_);
    }

    void Direct3DRender::ResizeRender(const RECT& render_rect) {
        if (::IsRectEmpty(&render_rect)) {
            return;
        }

        assert(d3d_device_);
        assert(d3d_immediate_context_);
        assert(d3d_swap_chain_);

        ReleaseCOMInterface(d3d_depth_stencil_buffer_);
        ReleaseCOMInterface(d3d_render_target_view_);
        ReleaseCOMInterface(d3d_depth_stencil_view_);

        width_ = render_rect.right - render_rect.left;
        height_ = render_rect.bottom - render_rect.top;
        HRESULT hr = d3d_swap_chain_->ResizeBuffers(1, width_, height_, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        Direct3DFailedDebugMsgBox(hr, , L"ResizeBuffers Failed.");

        ID3D11Texture2D* backBuffer = NULL;
        hr = d3d_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
        Direct3DFailedDebugMsgBox(hr, , L"Get back buffer Failed.");

        hr = d3d_device_->CreateRenderTargetView(backBuffer, 0, &d3d_render_target_view_);
        Direct3DFailedDebugMsgBox(hr, , L"CreateRenderTargetView Failed.");
        ReleaseCOMInterface(backBuffer);

        D3D11_TEXTURE2D_DESC depth_stencil_desc;
        depth_stencil_desc.Width = width_;
        depth_stencil_desc.Height = height_;
        depth_stencil_desc.MipLevels = 1;
        depth_stencil_desc.ArraySize = 1;
        depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_desc.SampleDesc.Count = 1;
        depth_stencil_desc.SampleDesc.Quality = 0;
        depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
        depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depth_stencil_desc.CPUAccessFlags = 0;
        depth_stencil_desc.MiscFlags = 0;

        hr = d3d_device_->CreateTexture2D(&depth_stencil_desc, 0, &d3d_depth_stencil_buffer_);
        Direct3DFailedDebugMsgBox(hr, , L"create depth stencil buffer failed.");

        hr = d3d_device_->CreateDepthStencilView(d3d_depth_stencil_buffer_, 0, &d3d_depth_stencil_view_);
        Direct3DFailedDebugMsgBox(hr, , L"create depth stencil view failed.");

        // Bind the render target view and depth/stencil view to the pipeline.
        d3d_immediate_context_->OMSetRenderTargets(1, &d3d_render_target_view_, d3d_depth_stencil_view_);

        d3d_screen_viewport_.TopLeftX = 0;
        d3d_screen_viewport_.TopLeftY = 0;
        d3d_screen_viewport_.Width = static_cast<float>(width_);
        d3d_screen_viewport_.Height = static_cast<float>(height_);
        d3d_screen_viewport_.MinDepth = 0.0f;
        d3d_screen_viewport_.MaxDepth = 1.0f;
        d3d_immediate_context_->RSSetViewports(1, &d3d_screen_viewport_);
    }

    void Direct3DRender::PresentScene() {
        assert(d3d_swap_chain_);

        //switch the back buffer and the front buffer
        d3d_swap_chain_->Present(0, 0);
    }

    bool Direct3DRender::IASetTextureLayout(const std::string& vertex_shader_file, const std::string& pixel_shader_file) {
        assert(d3d_device_);
        assert(d3d_immediate_context_);

        std::string vertex_shader_binary = LoadShader(vertex_shader_file);
        std::string pixel_shader_binary = LoadShader(pixel_shader_file);

        if (pixel_shader_binary.empty() || vertex_shader_binary.empty()) {
            return false;
        }

        ID3D11VertexShader* vertex_shader = NULL;
        HRESULT hr = d3d_device_->CreateVertexShader(vertex_shader_binary.data(), vertex_shader_binary.size(), NULL, &vertex_shader);
        Direct3DFailedDebugMsgBox(hr, false, L"create vertex shader failed.");
        d3d_immediate_context_->VSSetShader(vertex_shader, NULL, 0);

        ID3D11PixelShader* pixel_shader = NULL;
        hr = d3d_device_->CreatePixelShader(pixel_shader_binary.data(), pixel_shader_binary.size(), NULL, &pixel_shader);
        Direct3DFailedDebugMsgBox(hr, false, L"create pixel shader failed.");
        d3d_immediate_context_->PSSetShader(pixel_shader, NULL, 0);

        D3D11_INPUT_ELEMENT_DESC vertex_desc[] = {
            { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        int num = sizeof(vertex_desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
        ID3D11InputLayout* input_layout = NULL;
        hr = d3d_device_->CreateInputLayout(vertex_desc, num, vertex_shader_binary.data(), vertex_shader_binary.size(), &input_layout);
        Direct3DFailedDebugMsgBox(hr, false, L"create input layout failed.");
        d3d_immediate_context_->IASetInputLayout(input_layout);

        ReleaseCOMInterface(input_layout);
        ReleaseCOMInterface(pixel_shader);
        ReleaseCOMInterface(vertex_shader);

        return true;
    }

    bool Direct3DRender::IASetColorLayout() {
        assert(d3d_device_);
        assert(d3d_immediate_context_);

        std::string pixel_shader_binary = LoadShader("cp.dll");
        std::string vertex_shader_binary = LoadShader("cv.dll");

        if (pixel_shader_binary.empty() || vertex_shader_binary.empty()) {
            return false;
        }

        ID3D11VertexShader* vertex_shader = NULL;
        HRESULT hr = d3d_device_->CreateVertexShader(vertex_shader_binary.data(), vertex_shader_binary.size(), NULL, &vertex_shader);
        Direct3DFailedDebugMsgBox(hr, false, L"create vertex shader failed.");
        d3d_immediate_context_->VSSetShader(vertex_shader, NULL, 0);

        ID3D11PixelShader* pixel_shader = NULL;
        hr = d3d_device_->CreatePixelShader(pixel_shader_binary.data(), pixel_shader_binary.size(), NULL, &pixel_shader);
        Direct3DFailedDebugMsgBox(hr, false, L"create pixel shader failed.");
        d3d_immediate_context_->PSSetShader(pixel_shader, NULL, 0);

        D3D11_INPUT_ELEMENT_DESC vertex_desc[] = {
            { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        int num = sizeof(vertex_desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
        ID3D11InputLayout* input_layout = NULL;
        hr = d3d_device_->CreateInputLayout(vertex_desc, num, vertex_shader_binary.data(), vertex_shader_binary.size(), &input_layout);
        Direct3DFailedDebugMsgBox(hr, false, L"create input layout failed.");
        d3d_immediate_context_->IASetInputLayout(input_layout);

        ReleaseCOMInterface(input_layout);
        ReleaseCOMInterface(pixel_shader);
        ReleaseCOMInterface(vertex_shader);

        return true;
    }

    bool Direct3DRender::IASetRGBATextureLayout() {
        return IASetTextureLayout("rtv.dll", "rtp.dll");
    }

    bool Direct3DRender::IASetGrayTextureLayout() {
        return IASetTextureLayout("gtv.dll", "gtp.dll");
    }

    bool Direct3DRender::CreateTextureResource(const UINT width, const UINT height) {
        CD3D11_TEXTURE2D_DESC tex_desc(DXGI_FORMAT_R8_UNORM, width, height);
        tex_desc.MipLevels = 1;

        //UINT channels = sizeof(texture_planes_) / sizeof(ID3D11Texture2D*);
        for (auto& plane : texture_planes_) {
            HRESULT hr = d3d_device_->CreateTexture2D(&tex_desc, NULL, &plane);
            if (FAILED(hr)) {
                plane = NULL;
                return false;
            }
        }

        CD3D11_SHADER_RESOURCE_VIEW_DESC resource_view_desc(D3D11_SRV_DIMENSION_TEXTURE2D);
        UINT channels = sizeof(texture_resource_views_) / sizeof(ID3D11ShaderResourceView*);
        for (int i = 0; i < channels; ++i) {
            //给每一个纹理资源都创建一个纹理视图，这样资源才能被GPU访问: 真正被绑定到渲染管线的是资源视图，而不是资源
            HRESULT hr = d3d_device_->CreateShaderResourceView(texture_planes_[i], &resource_view_desc, &texture_resource_views_[i]);
            if (FAILED(hr)) {
                texture_resource_views_[i] = NULL;
                return false;
            }
        }

        return true;
    }

    bool Direct3DRender::UpdateTextureResource(const ImageData& image) {
        if (resource_width_ != image.width || resource_height_ != image.height) {
            UINT channels = sizeof(texture_resource_views_) / sizeof(ID3D11ShaderResourceView*);
            for (int i = 0; i < channels; ++i) {
                ReleaseCOMInterface(texture_planes_[i]);
                ReleaseCOMInterface(texture_resource_views_[i]);
            }

            if (!CreateTextureResource(image.width, image.height)) {
                return false;
            }
        }

        UINT channels = sizeof(texture_resource_views_) / sizeof(ID3D11ShaderResourceView*);
        if (channels != 4) {
            return false;
        }

        /*纹理资源更新方式：
        1.创建纹理时选择D3D11_USAGE_DEFAULT（GPU可读写）,然后只用UpdateSubresource更新纹理。
        2.创建纹理时选择D3D11_USAGE_DYNAMIC（GPU-R,CPU-W）加上D3D11_CPU_ACCESS_WRITE, 然后用 Map->memcpy->Unmap方式。
        第一种方式，即UpdateSubresource效率更高。
        */
        d3d_immediate_context_->UpdateSubresource(texture_planes_[0], 0, NULL, image.r.data(), image.width, 0);
        d3d_immediate_context_->UpdateSubresource(texture_planes_[1], 0, NULL, image.g.data(), image.width, 0);
        d3d_immediate_context_->UpdateSubresource(texture_planes_[2], 0, NULL, image.b.data(), image.width, 0);
        d3d_immediate_context_->UpdateSubresource(texture_planes_[3], 0, NULL, image.a.data(), image.width, 0);

        d3d_immediate_context_->PSSetShaderResources(0, channels, texture_resource_views_);

        return true;
    }

    bool Direct3DRender::SetLinearSamplerState() {
        // Create the sample state
        if (!linear_sampler_state_) {
            D3D11_SAMPLER_DESC sampler_desc = {};
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            sampler_desc.MinLOD = 0;
            sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
            HRESULT hr = d3d_device_->CreateSamplerState(&sampler_desc, &linear_sampler_state_);
            if (FAILED(hr)) {
                return false;
            }
        }

        d3d_immediate_context_->PSSetSamplers(0, 1, &linear_sampler_state_);

        return true;
    }

    bool Direct3DRender::FillColor(const RECT& rect, DWORD color) {
        return DrawRect(rect, color);
    }

    bool Direct3DRender::DrawImage(const RECT& item_rect, const RECT& paint_rect, ImageData& image) {
        if (!initialized_) {
            return false;
        }

        assert(d3d_device_);
        assert(d3d_immediate_context_);

        if (image.empty()) {
            bool load = LoadImage(image);
            if (!load) {
                return false;
            }
        }

        RECT rcDest = item_rect;
        //image.dest配置的区域是相对控件的
        if (image.dest.left != 0 || image.dest.top != 0 || image.dest.right != 0 || image.dest.bottom != 0) {
            rcDest.left = item_rect.left + image.dest.left;
            rcDest.top = item_rect.top + image.dest.top;
            
            rcDest.right = item_rect.left + image.dest.right;
            rcDest.right > item_rect.right ? rcDest.right = item_rect.right : NULL;

            rcDest.bottom = item_rect.top + image.dest.bottom;
            rcDest.bottom > item_rect.bottom ? rcDest.bottom = item_rect.bottom : NULL;
        }

        RECT temp_rect = { 0 };
        if (!::IntersectRect(&temp_rect, &rcDest, &item_rect)) {
            return false;
        }

        if (!::IntersectRect(&temp_rect, &rcDest, &paint_rect)) {
            return false;
        }
        
        if (!IASetTextureLayout("rtv.dll", "rtp.dll")) {
            return false;
        }
        UpdateTextureResource(image);

        if (image.alpha_blend || image.fade < 255) {
            //TODO: alpha blend

        }
        else {
            UINT index = 0;
            std::vector<TEXTURE_VERTEX> vertice;
            std::vector<WORD> indice;

            auto generate_vertice = [this, &vertice, &indice, &image](const RECT& dest, const RECT& source) {
                TEXTURE_VERTEX temp_vertice[] = {
                    XMFLOAT3(MapScreenX(dest.left, width_),  MapScreenY(dest.top, height_), 0.0f),
                    XMFLOAT2(MapTextureXY(source.left, image.width), MapTextureXY(source.top, image.height)), //left-top

                    XMFLOAT3(MapScreenX(dest.right, width_), MapScreenY(dest.top, height_), 0.0f),
                    XMFLOAT2(MapTextureXY(source.right, image.width), MapTextureXY(source.top, image.height)), //right-top

                    XMFLOAT3(MapScreenX(dest.right, width_), MapScreenY(dest.bottom, height_), 0.0f),
                    XMFLOAT2(MapTextureXY(source.right, image.width), MapTextureXY(source.bottom, image.height)), //right-bottom

                    XMFLOAT3(MapScreenX(dest.left, width_),  MapScreenY(dest.bottom, height_), 0.0f),
                    XMFLOAT2(MapTextureXY(source.left, image.width), MapTextureXY(source.bottom, image.height)), //left-bottom
                };

                WORD temp_indice[] = {
                    vertice.size(), vertice.size() + 1, vertice.size() + 2,
                    vertice.size(), vertice.size() + 2, vertice.size() + 3
                };

                for (auto& v : temp_vertice) {
                    vertice.push_back(v);
                }

                for (auto& i : temp_indice) {
                    indice.push_back(i);
                }
            };

            RECT temp_rect = { 0 };
            RECT draw_dest_rect = { 0 };
            RECT picture_source_rect = { 0 };

            // middle
            draw_dest_rect.left = rcDest.left + image.corner.left;
            draw_dest_rect.top = rcDest.top + image.corner.top;
            draw_dest_rect.right = rcDest.right - image.corner.right;
            draw_dest_rect.bottom = rcDest.bottom - image.corner.bottom;

            //去掉corner控制的间距区域
            picture_source_rect.left = image.source.left + image.corner.left;
            picture_source_rect.top = image.source.top + image.corner.top;
            picture_source_rect.right = image.source.right - image.corner.right;
            picture_source_rect.bottom = image.source.bottom - image.corner.bottom;

            if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                generate_vertice(draw_dest_rect, picture_source_rect);
            }

            // left-top corner
            if (image.corner.left > 0 && image.corner.top > 0) {
                draw_dest_rect.left = rcDest.left;
                draw_dest_rect.top = rcDest.top;
                draw_dest_rect.right = rcDest.left + image.corner.left;
                draw_dest_rect.bottom = rcDest.top + image.corner.top;

                picture_source_rect.left = image.source.left;
                picture_source_rect.top = image.source.top;
                picture_source_rect.right = image.source.left + image.corner.left;
                picture_source_rect.bottom = image.source.top + image.corner.top;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // top
            if (image.corner.top > 0) {
                draw_dest_rect.left = rcDest.left + image.corner.left;
                draw_dest_rect.top = rcDest.top;
                draw_dest_rect.right = rcDest.right - image.corner.right;
                draw_dest_rect.bottom = rcDest.top + image.corner.top;

                picture_source_rect.left = image.source.left + image.corner.left;
                picture_source_rect.top = image.source.top;
                picture_source_rect.right = image.source.right - image.corner.right;
                picture_source_rect.bottom = image.source.top + image.corner.top;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // right-top corner
            if (image.corner.right > 0 && image.corner.top > 0) {
                draw_dest_rect.left = rcDest.right - image.corner.right;
                draw_dest_rect.top = rcDest.top;
                draw_dest_rect.right = rcDest.right;
                draw_dest_rect.bottom = rcDest.top + image.corner.top;

                picture_source_rect.left = image.source.right - image.corner.right;
                picture_source_rect.top = image.source.top;
                picture_source_rect.right = image.source.right;
                picture_source_rect.bottom = image.source.top + image.corner.top;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // left
            if (image.corner.left > 0) {
                draw_dest_rect.left = rcDest.left;
                draw_dest_rect.top = rcDest.top + image.corner.top;
                draw_dest_rect.right = rcDest.left + image.corner.left;
                draw_dest_rect.bottom = rcDest.bottom - image.corner.bottom;

                picture_source_rect.left = image.source.left;
                picture_source_rect.top = image.source.top + image.corner.top;
                picture_source_rect.right = image.source.left + image.corner.left;
                picture_source_rect.bottom = image.source.bottom - image.corner.bottom;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // right
            if (image.corner.right > 0) {
                draw_dest_rect.left = rcDest.right - image.corner.right;
                draw_dest_rect.top = rcDest.top + image.corner.top;
                draw_dest_rect.right = rcDest.right;
                draw_dest_rect.bottom = rcDest.bottom - image.corner.bottom;

                picture_source_rect.left = image.source.right - image.corner.right;
                picture_source_rect.top = image.source.top + image.corner.top;
                picture_source_rect.right = image.source.right;
                picture_source_rect.bottom = image.source.bottom - image.corner.bottom;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // left-bottom
            if (image.corner.left > 0 && image.corner.bottom > 0) {
                draw_dest_rect.left = rcDest.left;
                draw_dest_rect.top = rcDest.bottom - image.corner.bottom;
                draw_dest_rect.right = rcDest.left + image.corner.left;
                draw_dest_rect.bottom = rcDest.bottom;

                picture_source_rect.left = image.source.left;
                picture_source_rect.top = image.source.bottom - image.corner.bottom;
                picture_source_rect.right = image.source.left + image.corner.left;
                picture_source_rect.bottom = image.source.bottom;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // bottom
            if (image.corner.bottom > 0) {
                draw_dest_rect.left = rcDest.left + image.corner.left;
                draw_dest_rect.top = rcDest.bottom - image.corner.bottom;
                draw_dest_rect.right = rcDest.right - image.corner.right;
                draw_dest_rect.bottom = rcDest.bottom;

                picture_source_rect.left = image.source.left + image.corner.left;
                picture_source_rect.top = image.source.bottom - image.corner.bottom;
                picture_source_rect.right = image.source.right - image.corner.right;
                picture_source_rect.bottom = image.source.bottom;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            // right-bottom
            if (image.corner.right > 0 && image.corner.bottom > 0) {
                draw_dest_rect.left = rcDest.right - image.corner.right;
                draw_dest_rect.top = rcDest.bottom - image.corner.bottom;
                draw_dest_rect.right = rcDest.right;
                draw_dest_rect.bottom = rcDest.bottom;

                picture_source_rect.left = image.source.right - image.corner.right;
                picture_source_rect.top = image.source.bottom - image.corner.bottom;
                picture_source_rect.right = image.source.right;
                picture_source_rect.bottom = image.source.bottom;

                if (::IntersectRect(&temp_rect, &paint_rect, &draw_dest_rect)) {
                    generate_vertice(draw_dest_rect, picture_source_rect);
                }
            }

            D3D11_BUFFER_DESC vertex_buffer_desc;
            vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            vertex_buffer_desc.ByteWidth = sizeof(TEXTURE_VERTEX) * vertice.size();  //size of the buffer
            vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
            vertex_buffer_desc.CPUAccessFlags = 0;
            vertex_buffer_desc.MiscFlags = 0;
            vertex_buffer_desc.StructureByteStride = 0;
            //or we can use Map and UnMap
            D3D11_SUBRESOURCE_DATA vertex_data;
            vertex_data.pSysMem = vertice.data();
            vertex_data.SysMemPitch = 0;
            vertex_data.SysMemSlicePitch = 0;
            ID3D11Buffer *vertex_buffer = NULL;
            HRESULT hr = d3d_device_->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
            Direct3DFailedDebugMsgBox(hr, false, L"create color vertex buffer failed.";);


            D3D11_BUFFER_DESC index_buffer_desc;
            index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            index_buffer_desc.ByteWidth = sizeof(WORD) * indice.size();
            index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            index_buffer_desc.CPUAccessFlags = 0;
            index_buffer_desc.MiscFlags = 0;
            index_buffer_desc.StructureByteStride = 0;
            D3D11_SUBRESOURCE_DATA index_data;
            index_data.pSysMem = indice.data();
            index_data.SysMemPitch = 0;
            index_data.SysMemSlicePitch = 0;
            ID3D11Buffer* index_buffer = NULL;
            hr = d3d_device_->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer);
            Direct3DFailedDebugMsgBox(hr, false, L"create color index buffer failed.";);

            unsigned int stride = sizeof(TEXTURE_VERTEX);
            unsigned int offset = 0;
            d3d_immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
            d3d_immediate_context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
            d3d_immediate_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            d3d_immediate_context_->DrawIndexed(indice.size(), 0, 0);

            ReleaseCOMInterface(vertex_buffer);
        }

        return true;
    }

    void Direct3DRender::DrawStatusImage() {
        
    }

    void Direct3DRender::DrawText(const RECT& text_rect, const CDuiString& text, DWORD color) {
        
        FreeTypeFont font(L"TODO", 16, true);
        font.LoadFont();
        ImageData image;
        if (font.GetTextBitmap('X', image)) {
            DrawImage(text_rect, text_rect, image);
        }
    }

    bool Direct3DRender::DrawBorder(const RECT& item_rect, const UINT border_size, DWORD color) {
        if (::IsRectEmpty(&item_rect) || border_size <= 0) {
            return false;
        }

        //left border
        RECT left = { item_rect.left, item_rect.top, item_rect.left + border_size, item_rect.bottom };
        bool left_border_succeed = FillColor(left, color);

        //top border
        RECT top = { item_rect.left + border_size, item_rect.top, item_rect.right,item_rect.top + border_size };
        bool top_border_succeed = FillColor(top, color);

        //right border
        RECT right = { item_rect.right - border_size, item_rect.top + border_size, item_rect.right,item_rect.bottom };
        bool right_border_succeed = FillColor(right, color);

        //bottom border
        RECT bottom = { item_rect.left + border_size, item_rect.bottom - border_size, item_rect.right - border_size,item_rect.bottom };
        bool bottom_border_succeed = FillColor(bottom, color);

        return left_border_succeed && top_border_succeed && right_border_succeed && bottom_border_succeed;
    }

    bool Direct3DRender::LoadImage(ImageData& image) {
        // 1、aaa.jpg
        // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' scale9='0,0,0,0' 
        // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false' hsl='false'
        if (image.empty()) {
            if (image.sDrawString.IsEmpty()) {
                return false;
            }

            CDuiString sImageName = image.sDrawString;
            CDuiString sImageResType;

            CDuiString sItem;
            CDuiString sValue;
            LPTSTR pstr = NULL;
            LPCTSTR pstrImage = image.sDrawString.GetData();
            while (*pstrImage != _T('\0')) {
                sItem.Empty();
                sValue.Empty();
                //去掉key头部的不可显字符和空格
                while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) pstrImage = ::CharNext(pstrImage);

                //解析出key名称，如"dest"
                while (*pstrImage != _T('\0') && *pstrImage != _T('=') && *pstrImage > _T(' ')) {
                    LPTSTR pstrTemp = ::CharNext(pstrImage);
                    if (pstrImage != pstrTemp) {
                        sItem += *pstrImage++;
                    }
                }

                //可能在解析到'='之前遇到不可显字符或者结尾
                while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) pstrImage = ::CharNext(pstrImage);
                if (*pstrImage++ != _T('=')) break;

                //去掉value头部的不可显字符和空格
                while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) pstrImage = ::CharNext(pstrImage);
                if (*pstrImage++ != _T('\'')) break;

                //解析出value值，如：0,0,0,0
                while (*pstrImage != _T('\0') && *pstrImage != _T('\'')) {
                    LPTSTR pstrTemp = ::CharNext(pstrImage);
                    if (pstrImage != pstrTemp) {
                        sValue += *pstrImage++;
                    }
                }

                if (*pstrImage++ != _T('\'')) break;

                if (!sValue.IsEmpty()) {
                    if (sItem == _T("file")) {
                        sImageName = sValue;
                    }
                    //else if (sItem == _T("res")) {
                    //    bUseRes = true;
                    //    sImageName = sValue;
                    //}
                    //else if (sItem == _T("restype")) {
                    //    sImageResType = sValue;
                    //}
                    //else if (sItem == _T("color")) {
                    //    bUseRes = true;
                    //    sImageResType = RES_TYPE_COLOR;
                    //    sImageName = sValue;
                    //}
                    else if (sItem == _T("dest")) {
                        image.dest.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
                        image.dest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        image.dest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        image.dest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                    }
                    else if (sItem == _T("source")) {
                        image.source.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
                        image.source.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        image.source.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        image.source.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                    }
                    else if (sItem == _T("corner") || sItem == _T("scale9")) {
                        image.corner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
                        image.corner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        image.corner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        image.corner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                    }
                    else if (sItem == _T("mask")) {
                        if (sValue[0] == _T('#')) image.mask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
                        else image.mask = _tcstoul(sValue.GetData(), &pstr, 16);
                    }
                    else if (sItem == _T("fade")) {
                        image.fade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
                    }

                    //scale9绘制不绘制中间区域，没用到过，暂不处理
                    //else if (sItem == _T("hole")) {
                    //    image.bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                    //}

                    //普通平铺而不拉伸，没用到过，暂不处理
                    //else if (sItem == _T("xtiled")) {
                    //    image.bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                    //}
                    //else if (sItem == _T("ytiled")) {
                    //    image.bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                    //}
                    //else if (sItem == _T("hsl")) {
                    //    bUseHSL = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                    //}
                }
                if (*pstrImage++ != _T(' ')) break;
            }

            image.sImageName = sImageName;

            //TODO: image hash map
            //const TImageInfo* data = NULL;
            //if (bUseRes == false) {
            //    data = pManager->GetImageEx((LPCTSTR)sImageName, NULL, dwMask, bUseHSL);
            //}
            //else {
            //    data = pManager->GetImageEx((LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask, bUseHSL);
            //}
            //if (!data) return false;

            Direct3DImage::LoadImage(image.sImageName, L"", image.mask, image);
            if (image.source.left == 0 && image.source.right == 0 && image.source.top == 0 && image.source.bottom == 0) {
                image.source.right = image.width;
                image.source.bottom = image.height;
            }
        }

        image.source.right > image.width ? image.source.right = image.width : NULL;
        image.source.bottom > image.height ? image.source.bottom = image.height : NULL;

        return true;
    }

    bool Direct3DRender::DrawColorVertex(const std::vector<COLOR_VERTEX>& vertice, const std::vector<WORD>& indice,
        const D3D11_PRIMITIVE_TOPOLOGY topo) {

        if (!initialized_) {
            return false;
        }

        assert(d3d_device_);
        assert(d3d_immediate_context_);
        assert(width_);
        assert(height_);

        if (!IASetColorLayout()) {
            return false;
        }

        D3D11_BUFFER_DESC vertex_buffer_desc;
        vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_buffer_desc.ByteWidth = sizeof(COLOR_VERTEX) * vertice.size();  //size of the buffer
        vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
        vertex_buffer_desc.CPUAccessFlags = 0;
        vertex_buffer_desc.MiscFlags = 0;
        vertex_buffer_desc.StructureByteStride = 0;
        //or we can use Map and UnMap
        D3D11_SUBRESOURCE_DATA vertex_data;
        vertex_data.pSysMem = vertice.data();
        vertex_data.SysMemPitch = 0;
        vertex_data.SysMemSlicePitch = 0;

        ID3D11Buffer *vertex_buffer = NULL;
        HRESULT hr = d3d_device_->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
        Direct3DFailedDebugMsgBox(hr, false, L"create color vertex buffer failed.";);


        D3D11_BUFFER_DESC index_buffer_desc;
        index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        index_buffer_desc.ByteWidth = sizeof(WORD) * indice.size();
        index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        index_buffer_desc.CPUAccessFlags = 0;
        index_buffer_desc.MiscFlags = 0;
        index_buffer_desc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA index_data;
        index_data.pSysMem = indice.data();
        index_data.SysMemPitch = 0;
        index_data.SysMemSlicePitch = 0;

        ID3D11Buffer* index_buffer = NULL;
        hr = d3d_device_->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer);
        Direct3DFailedDebugMsgBox(hr, false, L"create color index buffer failed.";);

        unsigned int stride = sizeof(COLOR_VERTEX);
        unsigned int offset = 0;
        d3d_immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        d3d_immediate_context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
        d3d_immediate_context_->IASetPrimitiveTopology(topo);

        d3d_immediate_context_->DrawIndexed(indice.size(), 0, 0);

        ReleaseCOMInterface(index_buffer);
        ReleaseCOMInterface(vertex_buffer);

        return true;
    }

    bool Direct3DRender::DrawLine(const POINT& begin, const POINT& end, DWORD dwColor) {
        if (!initialized_) {
            return false;
        }

        float begin_x = MapScreenX(begin.x, width_);
        float begin_y = MapScreenY(begin.y, height_);
        float end_x = MapScreenX(end.x, width_);
        float end_y = MapScreenY(end.y, height_);

        XMFLOAT4 color(GETR(dwColor) / 255.0f, GETG(dwColor) / 255.0f, GETB(dwColor) / 255.0f, GETA(dwColor) / 255.0f);
        COLOR_VERTEX temp_v[] = {
            XMFLOAT3(begin_y,  begin_y, 0.0f), color,
            XMFLOAT3(end_x, end_y, 0.0f), color
        };
        WORD temp_i[] = {
            0,1
        };

        std::vector<COLOR_VERTEX> vertice(temp_v, temp_v + sizeof(temp_v) / sizeof(COLOR_VERTEX));
        std::vector<WORD> indice(temp_i, temp_i + sizeof(temp_i) / sizeof(WORD));

        return DrawColorVertex(vertice, indice, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    }

    bool Direct3DRender::DrawRect(const RECT& rect, DWORD dwColor) {
        XMFLOAT4 color(GETR(dwColor) / 255.0f, GETG(dwColor) / 255.0f, GETB(dwColor) / 255.0f, GETA(dwColor) / 255.0f);
        COLOR_VERTEX temp_v[] = {
            XMFLOAT3(MapScreenX(rect.left, width_),  MapScreenY(rect.top, height_), 0.0f), color, //left-top
            XMFLOAT3(MapScreenX(rect.right, width_), MapScreenY(rect.top, height_), 0.0f), color, //right-top
            XMFLOAT3(MapScreenX(rect.right, width_), MapScreenY(rect.bottom, height_), 0.0f), color, //right-bottom
            XMFLOAT3(MapScreenX(rect.left, width_),  MapScreenY(rect.bottom, height_), 0.0f), color //left-bottom
        };

        //d3d_immediate_context_->ClearRenderTargetView(d3d_render_target_view_, reinterpret_cast<const float*>(&color));

        WORD temp_i[] = {
            0,1,2,
            0,2,3
        };

        std::vector<COLOR_VERTEX> vertice(temp_v, temp_v + sizeof(temp_v) / sizeof(COLOR_VERTEX));
        std::vector<WORD> indice(temp_i, temp_i + sizeof(temp_i) / sizeof(WORD));
        
        return DrawColorVertex(vertice, indice);
    }   

    std::string Direct3DRender::LoadShader(const std::string& cso_file) {
        std::ifstream ifs;
        std::string shader;
        ifs.open(cso_file, std::ios::binary | std::ios::in);
        if (ifs.is_open()) {
            ifs.seekg(0, std::ios_base::end);
            int size = (int)ifs.tellg();
            ifs.seekg(0, std::ios_base::beg);

            shader.resize(size);
            ifs.read(&shader[0], size);
            ifs.close();
        }

        return shader;
    }
}
