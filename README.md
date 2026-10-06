# Dear ImGui Custom Extensions & Standalone Additions

This repository contains all custom extensions, widgets, backends, and examples that were previously merged directly into Dear ImGui source files.

By extracting these additions into standalone files, you can keep your official Dear ImGui repository completely clean (unmodified upstream `master` or `docking`), enabling seamless updates without merge conflicts.

---

## Directory Structure

```text
imgui-extensions/
│
├── imgui_custom.h               # Custom widgets, layout helpers, and drawing extensions
├── imgui_custom.cpp             # Implementation of all custom widgets
│
├── backends/                    # Custom platform backends & helpers
│   ├── imgui_impl_uwp.h         # Universal Windows Platform (UWP / WinRT) backend header
│   ├── imgui_impl_uwp.cpp       # Universal Windows Platform backend implementation
│   ├── imgui_impl_win32_hook.h  # Hooked Win32 input backend header
│   ├── imgui_impl_win32_hook.cpp# Hooked Win32 input backend implementation
│   ├── imgui_impl_win32_extra.h # VirtualKeyToImGuiKey helper header
│   ├── imgui_impl_win32_extra.cpp # VirtualKeyToImGuiKey implementation
│   ├── imgui_impl_x11.h         # Native X11 Linux backend header
│   └── imgui_impl_x11.cpp       # Native X11 Linux backend implementation
│
└── examples/
    ├── imgui_uwp_examples.sln   # Visual Studio solution for UWP examples
    ├── example_uwp_directx11/   # DirectX 11 UWP sample
    ├── example_uwp_directx12/   # DirectX 12 UWP sample
    ├── example_uwp_gamebar_directx12/ # Xbox Game Bar DirectX 12 sample
    ├── example_uwp_swapchainpanel_directx12/ # XAML SwapChainPanel DirectX 12 sample
    ├── packages/                # NuGet packages required for UWP samples
    └── custom_menu_demo/        # Example main.cpp demonstrating the custom widgets (tabs, splitter, key selector, etc.)
```

---

## How to Use in Your Project

### 1. Widgets & Helpers (`imgui_custom.h`, `imgui_custom.cpp`)
Add `imgui_custom.h` and `imgui_custom.cpp` to your build system alongside official Dear ImGui files (`imgui.cpp`, `imgui_widgets.cpp`, etc.).

Include the header wherever you use the custom widgets:
```cpp
#include "imgui.h"
#include "imgui_custom.h"
```

### 2. Available Custom Features

#### Layout & Navigation
* `ImGui::Splitter(name, split_vertically, thickness, &size)`: Interactive splitter widget for resizable multi-pane layouts.
* `ImGui::BeginWithIcon(name, p_open, flags, icon_texture, titleheight)`: Creates a window with custom icon in the title bar and optional custom title bar height.

#### Centered & Aligned Text / Labels
* `ImGui::TextCentered(fmt, ...)`: Centers formatted text horizontally within the current window.
* `ImGui::TextColored(ImU32 col, fmt, ...)`: Directly accepts an `ImU32` color (e.g. `IM_COL32(255, 0, 0, 255)`).
* `ImGui::LabelAligned(label, align)`: Renders text aligned within the available content region (both X and Y axis, e.g. `ImVec2(0.5f, 0.5f)` for center).
* `ImGui::ImageAlignedWithBg(tex_id, size, align, uv0, uv1, tint_col, border_col)`: Centers or aligns an image with optional border and tint.

#### Key & Input Controls
* `ImGui::SelectKey(label, &key)`: Key-bind selector button. Clicking it opens a modal prompt requesting the user to press a key (`ImGuiKey`).
* `ImGui::InputByte(label, &byte_val, step, step_fast, flags)`: 8-bit unsigned integer scalar input with decimal or hex display (`ImGuiInputTextFlags_CharsHexadecimal`).

#### Color Editing
* `ImGui::ColorEdit4(label, ImU32* col, flags)`: Color editor taking a pointer to `ImU32`.
* `ImGui::ColorEdit4LargePreview(label, col, flags)`: Color editor with full-width preview button bar.

#### Text Rendering & Metrics
* `ImGui::CalcTextSizeForced(size, text, ...)`: Calculates text size for a specific font pixel size.
* `ImGui::AddTextPasses(draw_list, font, font_size, pos, col, text_begin, text_end, wrap_width, clip_rect, drawcount)`: Draws text across multiple passes for bold/outline rendering.

---

## Custom Backends

* **UWP (`imgui_impl_uwp.*`)**: Enables Dear ImGui on Universal Windows Platform (UWP / WinRT apps, Xbox Game Bar, and XAML SwapChainPanel).
* **Win32 Hook (`imgui_impl_win32_hook.*`)**: WndProc hook helper for injecting ImGui into existing Win32 applications / games.
* **Win32 Extra (`imgui_impl_win32_extra.*`)**: `ImGui_ImplWin32_VirtualKeyToImGuiKey(wParam)` converts Win32 `VK_*` codes directly to `ImGuiKey`.
* **X11 (`imgui_impl_x11.*`)**: Native X11 Linux window manager backend.

