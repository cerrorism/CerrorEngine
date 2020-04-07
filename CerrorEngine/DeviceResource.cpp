#include "pch.h"
#include "DeviceResource.h"

#if defined(_DEBUG)
// Check for SDK Layer support.
inline bool SdkLayersAvailable() noexcept
{
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
        nullptr,                    // Any feature level will do.
        0,
        D3D11_SDK_VERSION,
        nullptr,                    // No need to keep the D3D device reference.
        nullptr,                    // No need to know the feature level.
        nullptr                     // No need to keep the D3D device context reference.
        );

    return SUCCEEDED(hr);
}
#endif

const D3D_FEATURE_LEVEL FeatureLevels[] =
{
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
};


using namespace winrt;

DeviceResource::DeviceResource() noexcept
{
}

void DeviceResource::SetWindow(IUnknown* window, int width, int height) noexcept
{
    window = window;

    outputSize.left = outputSize.top = 0;
    outputSize.right = width;
    outputSize.bottom = height;
}

void DeviceResource::createResource()
{
    unsigned int creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    if (SdkLayersAvailable()) {
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
    else {
        OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
    }
#endif

#if defined(_DEBUG)
    {
        com_ptr<IDXGIInfoQueue> dxgiInfoQueue;
        check_hresult(DXGIGetDebugInterface1(0, __uuidof(dxgiInfoQueue), dxgiInfoQueue.put_void()));

        dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

        DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
        {
            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
        };
        DXGI_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(hide);
        filter.DenyList.pIDList = hide;
        dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
    }
#endif
    check_hresult(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(dxgiFactory), dxgiFactory.put_void()));

    int featureLevelsCount = std::count_if(FeatureLevels, FeatureLevels + 9, [this](D3D_FEATURE_LEVEL level) { return level > minFeatureLevel; });
    com_ptr<ID3D11Device> device;
    com_ptr<ID3D11DeviceContext> context;
    check_hresult(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,              // Set debug and Direct2D compatibility flags.
        FeatureLevels,
        featureLevelsCount,
        D3D11_SDK_VERSION,
        device.put(),      // Returns the Direct3D device created.
        &featureLevel,         // Returns feature level of device created.
        context.put()      // Returns the device immediate context.
        ));
#ifndef NDEBUG
    com_ptr<ID3D11InfoQueue> d3dInfoQueue = device.as<ID3D11Debug>().as<ID3D11InfoQueue>();
#ifdef _DEBUG
    d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
    d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
    D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS };
    D3D11_INFO_QUEUE_FILTER filter = {};
    filter.DenyList.NumIDs = _countof(hide);
    filter.DenyList.pIDList = hide;
    d3dInfoQueue->AddStorageFilterEntries(&filter);
#endif
    d3dDevice = device.as<ID3D11Device3>();
    d3dContext = context.as<ID3D11DeviceContext2>();
}

inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
    default:                                return fmt;
    }
}

void DeviceResource::createWindowSizeResource()
{
    if (!window)
    {
        throw std::exception("Call SetWindow with a valid CoreWindow pointer");
    }

    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    d3dRenderTargetView = nullptr;
    d3dDepthStencilView = nullptr;
    renderTarget = nullptr;
    depthStencil = nullptr;
    d3dContext->Flush();

    // Determine the render target size in pixels.
    const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(outputSize.right - outputSize.left), 1u);
    const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(outputSize.bottom - outputSize.top), 1u);
    const DXGI_FORMAT bbf = NoSRGB(backBufferFormat);

    if (swapChain) {
        // If the swap chain already exists, resize it.
        check_hresult(swapChain->ResizeBuffers(
            backBufferCount,
            backBufferWidth,
            backBufferHeight,
            bbf,
            0u
            ));
    }
    else {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = bbf;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        // Create a swap chain for the window.
        com_ptr<IDXGISwapChain1> sc;
        check_hresult(dxgiFactory->CreateSwapChainForCoreWindow(
            d3dDevice.get(),
            window,
            &swapChainDesc,
            nullptr,
            sc.put()
            ));
        swapChain = sc.as<IDXGISwapChain3>();

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
        // ensures that the application will only render after each VSync, minimizing power consumption.
        check_hresult(d3dDevice.as<IDXGIDevice3>()->SetMaximumFrameLatency(1));
    }

    check_hresult(swapChain->GetBuffer(0, __uuidof(renderTarget), renderTarget.put_void()));
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, bbf);
    d3dRenderTargetView = nullptr;
    check_hresult(d3dDevice->CreateRenderTargetView(
        renderTarget.get(),
        &renderTargetViewDesc,
        d3dRenderTargetView.put()
        ));
    if (depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Create a depth stencil view for use with 3D rendering if needed.
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
            );
        depthStencil = nullptr;
        check_hresult(d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            depthStencil.put()
            ));

        d3dDepthStencilView = nullptr;
        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        check_hresult(d3dDevice->CreateDepthStencilView(
            depthStencil.get(),
            &depthStencilViewDesc,
            d3dDepthStencilView.put()
            ));
    }

    screenViewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        static_cast<float>(backBufferWidth),
        static_cast<float>(backBufferHeight)
        );
}
