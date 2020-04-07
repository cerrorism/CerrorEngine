#pragma once

class DeviceResource
{
public:
    DeviceResource() noexcept;
    void SetWindow(IUnknown* window, int width, int height) noexcept;
    void createResource();
    void createWindowSizeResource();
private:
    //store input from constructor to later create resource
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    unsigned int dxgiFactoryFlags = 0;
    unsigned int options = 0;
    D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_1;

    //the final output of device
    winrt::com_ptr<IDXGIFactory2> dxgiFactory = nullptr;
    winrt::com_ptr<ID3D11Device3> d3dDevice = nullptr;
    winrt::com_ptr<ID3D11DeviceContext2> d3dContext = nullptr;

    IUnknown* window = nullptr;
    RECT outputSize = {};
    winrt::com_ptr<ID3D11Texture2D> renderTarget = nullptr;
    winrt::com_ptr<ID3D11Texture2D> depthStencil = nullptr;
    winrt::com_ptr<ID3D11RenderTargetView> d3dRenderTargetView = nullptr;
    winrt::com_ptr<ID3D11DepthStencilView> d3dDepthStencilView = nullptr;
    winrt::com_ptr<IDXGISwapChain3> swapChain = nullptr;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    int backBufferCount = 2;
    D3D11_VIEWPORT screenViewport = {};
};

