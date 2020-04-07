#pragma once

#include "DeviceResource.h"

class Game final
{
public:
	Game() noexcept(false);
	~Game() = default;

	// Initialization and management
	void Initialize(IUnknown* window, int width, int height);
	void GetDefaultSize(int& width, int& height);
	void OnActivated();
	void OnDeactivated();
private:
	std::unique_ptr<DeviceResource> deviceResource = std::make_unique<DeviceResource>();
	IUnknown* window;
	
};

