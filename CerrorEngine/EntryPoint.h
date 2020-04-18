#pragma once
#include "pch.h"

using winrt::Windows::ApplicationModel::Core::IFrameworkViewSource;
using winrt::Windows::ApplicationModel::Core::IFrameworkView;

class EntryPoint : public winrt::implements<EntryPoint, IFrameworkViewSource>
{
public:
    IFrameworkView CreateView();
};

