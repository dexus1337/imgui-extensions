// dear imgui - Custom Extensions & Widgets Implementation
#include "imgui_custom.h"
#include <float.h>

//-----------------------------------------------------------------------------
// [SECTION] Splitters
//-----------------------------------------------------------------------------

void ImGui::Splitter(const char* name, bool split_vertically, float thickness, float* size)
{
    ImGui::SameLine();
    ImGui::Button(name, ImVec2(split_vertically ? ImGui::GetContentRegionAvail().x : thickness,
                               split_vertically ? thickness : ImGui::GetContentRegionAvail().y));
    if (ImGui::IsItemActive())
        *size += split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
    ImGui::SameLine();
}

//-----------------------------------------------------------------------------
// [SECTION] Centered & Aligned Text
//-----------------------------------------------------------------------------

void ImGui::TextCentered(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextCenteredV(fmt, args);
    va_end(args);
}

void ImGui::TextCenteredV(const char* fmt, va_list args)
{
    const char* text;
    const char* text_end;
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);

    const float windowWidth = ImGui::GetWindowSize().x;
    const float textWidth = ImGui::CalcTextSize(text, text_end).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    TextEx(text, text_end, ImGuiTextFlags_None);
}

void ImGui::TextColored(ImU32 col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}

void ImGui::TextColoredV(ImU32 col, const char* fmt, va_list args)
{
    PushStyleColor(ImGuiCol_Text, col);
    TextV(fmt, args);
    PopStyleColor();
}

void ImGui::LabelAligned(const char* label, const ImVec2& align)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImVec2 avail = GetContentRegionAvail();
    const ImVec2 cursor = window->DC.CursorPos;
    const ImVec2 text_size = CalcTextSize(label, nullptr, true);

    // Offsets relative to the remaining region
    const float off_x = (avail.x - text_size.x) * align.x;
    const float off_y = (avail.y - text_size.y) * align.y;

    // Where we will actually render the text
    const ImVec2 pos = ImVec2(cursor.x + off_x, cursor.y + off_y);

    // The item's visual bounding box
    const ImRect bb(pos, pos + text_size);

    // Reserved layout box
    const float reserve_w = text_size.x + ImMax(0.0f, -off_x);
    const float reserve_h = text_size.y + ImMax(0.0f, -off_y);
    const ImRect reserve_bb(cursor, cursor + ImVec2(reserve_w, reserve_h));

    ItemSize(reserve_bb);
    if (!ItemAdd(bb, window->GetID(label)))
        return;

    RenderText(pos, label);
}

//-----------------------------------------------------------------------------
// [SECTION] Aligned Images
//-----------------------------------------------------------------------------

void ImGui::ImageAlignedWithBg(ImTextureID tex_id, const ImVec2& image_size, const ImVec2& align,
                               const ImVec2& uv0, const ImVec2& uv1,
                               const ImVec4& tint_col, const ImVec4& border_col)
{
    const float windowWidth = ImGui::GetWindowSize().x - ImGui::GetCursorPosX();
    const float windowHeight = ImGui::GetWindowSize().y - ImGui::GetCursorPosY();

    if (align.x != 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (windowWidth * align.x) - (image_size.x * align.x));

    if (align.y != 0.0f)
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (windowHeight * align.y) - (image_size.y * align.y));

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const float border_size = (border_col.w > 0.0f) ? 1.0f : 0.0f;
    const ImVec2 padding(border_size, border_size);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + image_size + padding * 2.0f);
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
        return;

    if (border_size > 0.0f)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f, ImDrawFlags_None, border_size);

    window->DrawList->AddImage(tex_id, bb.Min + padding, bb.Max - padding, uv0, uv1, GetColorU32(tint_col));
}

//-----------------------------------------------------------------------------
// [SECTION] Input Helpers
//-----------------------------------------------------------------------------

bool ImGui::InputByte(const char* label, unsigned char* v, unsigned char step, unsigned char step_fast, ImGuiInputTextFlags flags)
{
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%01X" : "%d";
    return InputScalar(label, ImGuiDataType_U8, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] Color Editors
//-----------------------------------------------------------------------------

bool ImGui::ColorEdit4(const char* label, ImU32* col, ImGuiColorEditFlags flags)
{
    if (!col)
        return false;

    ImVec4 src = ColorConvertU32ToFloat4(*col);
    bool result = ColorEdit4(label, reinterpret_cast<float*>(&src), flags);
    *col = ColorConvertFloat4ToU32(src);
    return result;
}

bool ImGui::ColorEdit4LargePreview(const char* label, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_total = CalcItemWidth();
    const ImVec2 pos = window->DC.CursorPos;

    const ImVec4 col_v4(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
    const ImVec2 button_sz = ImVec2(w_total, square_sz);

    bool value_changed = false;
    if (ColorButton("##ColorButton", col_v4, flags, button_sz))
    {
        if (!(flags & ImGuiColorEditFlags_NoPicker))
            OpenPopup("picker");
    }

    if (!(flags & ImGuiColorEditFlags_NoOptions))
        OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

    if (BeginPopup("picker"))
    {
        ImGuiColorEditFlags picker_flags = (flags & ~ImGuiColorEditFlags_LargeColorOnly);
        value_changed |= ColorPicker4("##picker", col, picker_flags);
        EndPopup();
    }

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (label_size.x > 0.0f)
    {
        SameLine(0.0f, style.ItemInnerSpacing.x);
        TextEx(label);
    }

    return value_changed;
}

bool ImGui::ColorEdit4LargePreview(const char* label, ImU32* col, ImGuiColorEditFlags flags)
{
    if (!col)
        return false;

    ImVec4 src = ColorConvertU32ToFloat4(*col);
    bool result = ColorEdit4LargePreview(label, reinterpret_cast<float*>(&src), flags);
    *col = ColorConvertFloat4ToU32(src);
    return result;
}

//-----------------------------------------------------------------------------
// [SECTION] Key Selector Widget
//-----------------------------------------------------------------------------

bool ImGui::SelectKey(const char* label, ImGuiKey* p_key, const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float w = (size.x > 0.0f) ? size.x : CalcItemWidth();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float h = (size.y > 0.0f) ? size.y : (label_size.y + style.FramePadding.y * 2.0f);
    const ImVec2 btn_size = ImVec2(w, h);

    bool changed = false;
    const char* key_name = (p_key && *p_key != ImGuiKey_None) ? GetKeyName(*p_key) : "NONE";

    if (Button(key_name, btn_size))
    {
        ImGui::OpenPopup("SELECT KEY##press_key_select_key", ImGuiPopupFlags_NoReopen);
    }

    if (ImGui::BeginPopupModal("SELECT KEY##press_key_select_key", NULL,
                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::GetIO().MouseDrawCursor = false;
        ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y / 2.0f);

        for (int i = ImGuiKey_NamedKey_END - 1; i >= ImGuiKey_NamedKey_BEGIN; i--)
        {
            if (ImGui::IsKeyDown(static_cast<ImGuiKey>(i)))
            {
                if (p_key)
                    *p_key = static_cast<ImGuiKey>(i);

                changed = true;
                ImGui::CloseCurrentPopup();
                ImGui::GetIO().MouseDrawCursor = true;
                break;
            }
        }

        ImGui::SeparatorText("");
        ImGui::Spacing();
        ImGui::TextCentered("PLEASE PRESS A KEY...");
        ImGui::Spacing();
        ImGui::SeparatorText("");
        ImGui::EndPopup();
    }

    if (label_size.x > 0.0f)
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(label, FindRenderedTextEnd(label));
    }

    return changed;
}

//-----------------------------------------------------------------------------
// [SECTION] Text Metrics
//-----------------------------------------------------------------------------

ImVec2 ImGui::CalcTextSizeForced(int size, const char* text, const char* text_end, bool hide_text_after_double_hash)
{
    ImGuiContext& g = *GImGui;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindRenderedTextEnd(text, text_end);
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    if (text == text_display_end)
        return ImVec2(0.0f, (float)size);

    ImVec2 text_size = font->CalcTextSizeA((float)size, FLT_MAX, -1.0f, text, text_display_end, NULL);
    text_size.x = IM_TRUNC(text_size.x + 0.99999f);
    return text_size;
}

//-----------------------------------------------------------------------------
// [SECTION] Multi-pass Text Drawing
//-----------------------------------------------------------------------------

void ImGui::AddTextPasses(ImDrawList* draw_list, ImFont* font, float font_size, const ImVec2& pos, ImU32 col,
                          const char* text_begin, const char* text_end, float wrap_width,
                          const ImVec4* cpu_fine_clip_rect, int drawcount)
{
    if ((col & IM_COL32_A_MASK) == 0 || draw_list == NULL)
        return;

    if (font == NULL)
        font = GImGui->Font;
    if (font_size == 0.0f)
        font_size = GImGui->FontSize;

    ImVec4 clip_rect = draw_list->_ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }

    for (; drawcount > 0; drawcount--)
    {
        font->RenderText(draw_list, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width,
                         (cpu_fine_clip_rect != NULL) ? ImDrawTextFlags_CpuFineClip : ImDrawTextFlags_None);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Window With Icon & Custom Title Bar Height
//-----------------------------------------------------------------------------

bool ImGui::BeginWithIcon(const char* name, bool* p_open, ImGuiWindowFlags flags,
                          ImTextureID icon_texture, int titleheight)
{
    ImGuiContext& g = *GImGui;
    bool open = ImGui::Begin(name, p_open, flags);
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if (window && titleheight > 0 && !(flags & ImGuiWindowFlags_NoTitleBar))
    {
        window->TitleBarHeight = (float)titleheight + g.Style.FramePadding.y * 2.0f;
    }

    if (window && icon_texture != 0 && !(flags & ImGuiWindowFlags_NoTitleBar) && !window->Collapsed)
    {
        const float iconsize = window->TitleBarHeight - (g.Style.FramePadding.y * 2.0f);
        const ImVec2 pos(window->Pos.x + g.Style.FramePadding.x, window->Pos.y + g.Style.FramePadding.y);
        const ImVec2 pos_max(pos.x + iconsize, pos.y + iconsize);
        window->DrawList->AddImageRounded(icon_texture, pos, pos_max, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0xffffffff, g.Style.FrameRounding);
    }

    return open;
}

