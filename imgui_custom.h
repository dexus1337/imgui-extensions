// dear imgui - Custom Extensions & Widgets
// Extracted standalone custom additions (previously merged into imgui core files)
// Include this header alongside "imgui.h" and "imgui_internal.h".

#pragma once

#include "imgui.h"

//-----------------------------------------------------------------------------
// [SECTION] Custom Flags & Constants
//-----------------------------------------------------------------------------

typedef int ImGuiCenteredFlags;
enum ImGuiCenteredFlags_
{
    ImGuiCenteredFlags_Horizontal           = 1 << 0,
    ImGuiCenteredFlags_Vertical             = 1 << 1,
    ImGuiCenteredFlags_HorizontalVertical   = ImGuiCenteredFlags_Horizontal | ImGuiCenteredFlags_Vertical
};

typedef int ImGuiIconPosition;
enum ImGuiIconPosition_
{
    ImGuiIconPosition_Left          = 0, // Far-left of title bar (after collapse button if present)
    ImGuiIconPosition_Right         = 1, // Far-right of title bar (before close/collapse button)
    ImGuiIconPosition_TitleTextLeft = 2, // Immediately to the left of the title text (respects style.WindowTitleAlign)
};

#ifndef ImGuiMouseButton_X1
#define ImGuiMouseButton_X1 3
#endif

#ifndef ImGuiMouseButton_X2
#define ImGuiMouseButton_X2 4
#endif

//-----------------------------------------------------------------------------
// [SECTION] Custom Widgets & Helpers (Namespace ImGui)
//-----------------------------------------------------------------------------

namespace ImGui
{
    // --- Layout & Splitters ---
    // Draggable splitter between regions / child windows
    IMGUI_API void Splitter(const char* name, bool split_vertically, float thickness, float* size);

    // --- Centered / Aligned Text ---
    // Displays horizontally centered text relative to the current window
    IMGUI_API void TextCentered(const char* fmt, ...) IM_FMTARGS(1);
    IMGUI_API void TextCenteredV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Displays text with ImU32 color directly (avoiding ImVec4 conversion overhead)
    IMGUI_API void TextColored(ImU32 col, const char* fmt, ...) IM_FMTARGS(2);
    IMGUI_API void TextColoredV(ImU32 col, const char* fmt, va_list args) IM_FMTLIST(2);

    // Displays aligned text within size_x (align_x: 0.0=left, 0.5=center, 1.0=right)
    IMGUI_API void TextAligned(float align_x, float size_x, const char* fmt, ...) IM_FMTARGS(3);
    IMGUI_API void TextAlignedV(float align_x, float size_x, const char* fmt, va_list args) IM_FMTLIST(3);

    // Displays text aligned within the available content region (both X and Y alignment)
    IMGUI_API void LabelAligned(const char* label, const ImVec2& align = ImVec2(0.5f, 0.5f));

    // --- Aligned Images ---
    // Displays image aligned within available region with background / border support
    IMGUI_API void ImageAlignedWithBg(ImTextureID tex_id, const ImVec2& image_size, const ImVec2& align = ImVec2(0.5f, 0.5f),
                                      const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
                                      const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

    // --- Inputs ---
    // Byte (unsigned char) scalar input supporting hex (%01X) and decimal
    IMGUI_API bool InputByte(const char* label, unsigned char* v, unsigned char step = 1, unsigned char step_fast = 1, ImGuiInputTextFlags flags = 0);

    // --- Color Editors ---
    // Color editor with full-width large preview button (supporting float[4] and ImU32 color)
    IMGUI_API bool ColorEdit4LargePreview(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit4LargePreview(const char* label, ImU32* col, ImGuiColorEditFlags flags = 0);

    // --- Key Selector ---
    // Interactive button that opens a modal popup to capture and assign a keypress (ImGuiKey)
    IMGUI_API bool SelectKey(const char* label, ImGuiKey* p_key, const ImVec2& size = ImVec2(0, 0));

    // --- Text Metrics ---
    // Calculate text size with forced font pixel height
    IMGUI_API ImVec2 CalcTextSizeForced(int size, const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false);

    // --- Multi-pass Text Drawing ---
    // Renders text multiple times (drawcount passes) for bolding/emphasis
    IMGUI_API void AddTextPasses(ImDrawList* draw_list, ImFont* font, float font_size, const ImVec2& pos, ImU32 col,
                                const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f,
                                const ImVec4* cpu_fine_clip_rect = NULL, int drawcount = 1);

    // --- Windows with Icon / Custom Titlebar Height ---
    // Begins a window with an icon in the titlebar, optional custom titlebar height and icon position
    IMGUI_API bool BeginWithIcon(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0,
                                 ImTextureID icon_texture = 0, int titleheight = 0,
                                 ImGuiIconPosition icon_pos = ImGuiIconPosition_Left);
}

