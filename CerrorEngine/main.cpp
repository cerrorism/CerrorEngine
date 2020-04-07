#include "pch.h"
#include "EntryPoint.h"

using namespace winrt;
using winrt::Windows::ApplicationModel::Core::CoreApplication;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<EntryPoint>());
}