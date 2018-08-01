#include "stdafx.h"
#include "D3DGraphicEngine.h"
#include <Gdiplus.h>
#include <fstream>
#include "D3DTypes.h"

using namespace DirectX;

namespace DuiLib {

    D3DGraphicEngine::D3DGraphicEngine() {

    }


    D3DGraphicEngine::~D3DGraphicEngine() {

    }

    bool D3DGraphicEngine::InitDirect3D(HWND render_window) {
        if (!render_window) {
            return false;
        }

        UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        //������Բ㡣��ָ�����Ա�־ֵ��Direct3D����VC++��������ڷ��͵�����Ϣ
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

        return true;
    }

    void D3DGraphicEngine::ReleaseDirect3D() {
        ReleaseCOMInterface(d3d_swap_chain_);
        ReleaseCOMInterface(d3d_immediate_context_);
        ReleaseCOMInterface(d3d_device_);
    }

    void D3DGraphicEngine::ResizeRender(const RECT& render_rect) {
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

    void D3DGraphicEngine::PresentScene() {
        assert(d3d_swap_chain_);

        //switch the back buffer and the front buffer
        d3d_swap_chain_->Present(0, 0);
    }

    void D3DGraphicEngine::DrawColor(const RECT& render_rect, DWORD dwcolor) {
        assert(d3d_device_);
        assert(d3d_immediate_context_);
        assert(width_);
        assert(height_);

        if (!IASetColorLayout()) {
            return;
        }

        Color c(dwcolor);
        XMFLOAT4 color(c.GetR() / 255.0f, c.GetG() / 255.0f, c.GetB() / 255.0f, c.GetA() / 255.0f);

        COLOR_VERTEX vertice[] = {
            XMFLOAT3(MapScreenX(render_rect.left, width_),  MapScreenY(render_rect.top, height_), 0.0f), color, //left-top
            XMFLOAT3(MapScreenX(render_rect.right, width_), MapScreenY(render_rect.top, height_), 0.0f), color, //right-top
            XMFLOAT3(MapScreenX(render_rect.right, width_), MapScreenY(render_rect.bottom, height_), 0.0f), color, //right-bottom
            XMFLOAT3(MapScreenX(render_rect.left, width_),  MapScreenY(render_rect.bottom, height_), 0.0f), color, //left-bottom
        };

        //d3d_immediate_context_->ClearRenderTargetView(d3d_render_target_view_, reinterpret_cast<const float*>(&color));

        WORD indice[] = {
            0,1,2,
            0,2,3
        };

        D3D11_BUFFER_DESC vertex_buffer_desc;
        vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_buffer_desc.ByteWidth = sizeof(vertice);  //size of the buffer
        vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
        vertex_buffer_desc.CPUAccessFlags = 0;
        vertex_buffer_desc.MiscFlags = 0;
        vertex_buffer_desc.StructureByteStride = 0;
        //or we can use Map and UnMap
        D3D11_SUBRESOURCE_DATA vertex_data;
        vertex_data.pSysMem = vertice;
        vertex_data.SysMemPitch = 0;
        vertex_data.SysMemSlicePitch = 0;
        ID3D11Buffer *vertex_buffer = NULL;
        HRESULT hr = d3d_device_->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
        Direct3DFailedDebugMsgBox(hr, , L"create color vertex buffer failed.";);


        D3D11_BUFFER_DESC index_buffer_desc;
        index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        index_buffer_desc.ByteWidth = sizeof(indice);
        index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        index_buffer_desc.CPUAccessFlags = 0;
        index_buffer_desc.MiscFlags = 0;
        index_buffer_desc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA index_data;
        index_data.pSysMem = indice;
        index_data.SysMemPitch = 0;
        index_data.SysMemSlicePitch = 0;
        ID3D11Buffer* index_buffer = NULL;
        hr = d3d_device_->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer);
        Direct3DFailedDebugMsgBox(hr, , L"create color index buffer failed.";);

        unsigned int stride = sizeof(COLOR_VERTEX);
        unsigned int offset = 0;
        d3d_immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        d3d_immediate_context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
        d3d_immediate_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        d3d_immediate_context_->DrawIndexed(sizeof(indice) / sizeof(WORD), 0, 0);

        ReleaseCOMInterface(vertex_buffer);
    }

    bool D3DGraphicEngine::IASetColorLayout() {
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

    std::string D3DGraphicEngine::LoadShader(const std::string& cso_file) {
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
