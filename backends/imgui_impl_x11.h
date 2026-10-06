// dear imgui: Platform Backend for X11 Linux Windows Manager
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// If you dont want to use the Xfixes lib, define IMGUI_IMPL_X11_NO_XFIXES

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE

IMGUI_IMPL_API bool     ImGui_ImplX11_Init(void* display, int window);
IMGUI_IMPL_API bool     ImGui_ImplX11_OnEvent(void* event);
IMGUI_IMPL_API void     ImGui_ImplX11_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplX11_NewFrame();

#endif // #ifndef IMGUI_DISABLE
