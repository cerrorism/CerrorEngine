#include "pch.h"

#include "App.h"
#include "Game.h"
using namespace winrt;


using winrt::Windows::ApplicationModel::Activation::ActivationKind;
using winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs;
using winrt::Windows::UI::Core::CoreDispatcher;
using winrt::Windows::UI::Core::CoreProcessEventsOption;
using winrt::Windows::UI::Core::CoreAcceleratorKeyEventType;
using winrt::Windows::ApplicationModel::Core::CoreApplication;
using winrt::Windows::Graphics::Display::DisplayInformation;
using winrt::Windows::UI::ViewManagement::ApplicationView;
using winrt::Windows::UI::ViewManagement::ApplicationViewWindowingMode;
using winrt::Windows::UI::ViewManagement::FullScreenSystemOverlayMode;
using winrt::Windows::Foundation::Size;
using winrt::Windows::System::VirtualKey;

void App::Initialize(CoreApplicationView const& applicationView) {
    applicationView.Activated({ this, &App::onActivated });
    game = std::make_unique<Game>();
}

void App::SetWindow(CoreWindow const& window)
{
    window.Closed([this](auto&&, auto&&) { exit = true; });
    auto windowPtr = static_cast<IUnknown*>(winrt::get_abi(window));
    int outputWidth = ConvertDipsToPixels(logicalWidth);
    int outputHeight = ConvertDipsToPixels(logicalHeight);
    game->Initialize(windowPtr, outputWidth, outputHeight);
}

void App::Load(hstring const&)
{
}

void App::Run()
{
    while (!exit)
    {
        CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }
}

void App::Uninitialize()
{
}

void App::onActivated(CoreApplicationView const&, IActivatedEventArgs const&)
{
    int w, h;
    game->GetDefaultSize(w, h);
    dpi = DisplayInformation::GetForCurrentView().LogicalDpi();
    ApplicationView::PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
    auto desiredSize = Size(ConvertPixelsToDips(w), ConvertPixelsToDips(h));
    auto view = ApplicationView::GetForCurrentView();
    CoreWindow::GetForCurrentThread().Activate();
    view.FullScreenSystemOverlayMode(FullScreenSystemOverlayMode::Minimal);
    view.TryResizeView(desiredSize);
}
