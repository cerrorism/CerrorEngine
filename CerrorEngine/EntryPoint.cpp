#include "pch.h"
#include "EntryPoint.h"
#include "App.h"

using namespace winrt;
using winrt::Windows::ApplicationModel::Core::CoreApplication;


IFrameworkView EntryPoint::CreateView()
{
    return make<App>();
}