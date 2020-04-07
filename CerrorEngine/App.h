#pragma once
#include "pch.h"

#include "Game.h"

using winrt::Windows::ApplicationModel::Core::IFrameworkView;
using winrt::Windows::ApplicationModel::Core::CoreApplicationView;
using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::WindowSizeChangedEventArgs;
using winrt::Windows::UI::Core::VisibilityChangedEventArgs;
using winrt::Windows::UI::Core::AcceleratorKeyEventArgs;
using winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs;

class App : public winrt::implements<App, IFrameworkView> {
public:
	void Initialize(CoreApplicationView const&);
	void SetWindow(CoreWindow const&);
	void Load(winrt::hstring const&);
	void Run();
	void Uninitialize();

protected:
	void onActivated(CoreApplicationView const& /*applicationView*/, IActivatedEventArgs const& args);

private:
	std::unique_ptr<Game> game;
	float dpi;
	float logicalWidth;
	float logicalHeight;
	bool exit = false;

	inline float ConvertPixelsToDips(int pixels) const noexcept
	{
		return (float(pixels) * 96.f / dpi);
	}

	inline int ConvertDipsToPixels(float dips) const noexcept
	{
		return int(dips * dpi / 96.f + 0.5f);
	}
};