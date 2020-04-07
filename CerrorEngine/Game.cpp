#include "pch.h"
#include "Game.h"

Game::Game() noexcept(false): window(nullptr)
{
}

void Game::Initialize(IUnknown* window, int width, int height)
{
	deviceResource->SetWindow(window, width, height);
	deviceResource->createResource();
}

void Game::GetDefaultSize(int& width, int& height)
{
	width = 800;
	height = 600;
}

void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}
