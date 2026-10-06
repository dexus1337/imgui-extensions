// dear imgui: Helpers for Win32 Backend
#pragma once
#include <stdint.h>
#include "imgui.h"

IMGUI_IMPL_API ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(uintptr_t wParam);

