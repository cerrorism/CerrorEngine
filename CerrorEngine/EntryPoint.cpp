#include "pch.h"
#include "EntryPoint.h"
#include "App.h"

using namespace winrt;
using Windows::ApplicationModel::Core::CoreApplication;


IFrameworkView EntryPoint::CreateView()
{
    return make<App>();
}