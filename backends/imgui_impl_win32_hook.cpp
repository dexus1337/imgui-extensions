#include "imgui_impl_win32_hook.h"

#include "input.hpp"

#include <Windows.h>

// There is no distinct VK_xxx for keypad enter, instead it is VK_RETURN + KF_EXTENDED, we assign it an arbitrary value to make code more readable (VK_ codes go up to 255)
#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)

struct ImGui_ImplWin32_Data {
    HWND hWnd;
    HWND MouseHwnd;
    bool MouseTracked;
    int MouseButtonsDown;
    INT64 Time;
    INT64 TicksPerSecond;
    ImGuiMouseCursor LastMouseCursor;
    bool HasGamepad;
    bool WantUpdateHasGamepad;

#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    HMODULE XInputDLL;
    void* XInputGetCapabilities;
    void* XInputGetState;
#endif

    ImGui_ImplWin32_Data() { memset((void*) this, 0, sizeof(*this)); }
};

ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(WPARAM wParam) {
    switch (wParam) {
        case VK_TAB:
            return ImGuiKey_Tab;
        case VK_LEFT:
            return ImGuiKey_LeftArrow;
        case VK_RIGHT:
            return ImGuiKey_RightArrow;
        case VK_UP:
            return ImGuiKey_UpArrow;
        case VK_DOWN:
            return ImGuiKey_DownArrow;
        case VK_PRIOR:
            return ImGuiKey_PageUp;
        case VK_NEXT:
            return ImGuiKey_PageDown;
        case VK_HOME:
            return ImGuiKey_Home;
        case VK_END:
            return ImGuiKey_End;
        case VK_INSERT:
            return ImGuiKey_Insert;
        case VK_DELETE:
            return ImGuiKey_Delete;
        case VK_BACK:
            return ImGuiKey_Backspace;
        case VK_SPACE:
            return ImGuiKey_Space;
        case VK_RETURN:
            return ImGuiKey_Enter;
        case VK_ESCAPE:
            return ImGuiKey_Escape;
        case VK_OEM_7:
            return ImGuiKey_Apostrophe;
        case VK_OEM_COMMA:
            return ImGuiKey_Comma;
        case VK_OEM_MINUS:
            return ImGuiKey_Minus;
        case VK_OEM_PERIOD:
            return ImGuiKey_Period;
        case VK_OEM_2:
            return ImGuiKey_Slash;
        case VK_OEM_1:
            return ImGuiKey_Semicolon;
        case VK_OEM_PLUS:
            return ImGuiKey_Equal;
        case VK_OEM_4:
            return ImGuiKey_LeftBracket;
        case VK_OEM_5:
            return ImGuiKey_Backslash;
        case VK_OEM_6:
            return ImGuiKey_RightBracket;
        case VK_OEM_3:
            return ImGuiKey_GraveAccent;
        case VK_CAPITAL:
            return ImGuiKey_CapsLock;
        case VK_SCROLL:
            return ImGuiKey_ScrollLock;
        case VK_NUMLOCK:
            return ImGuiKey_NumLock;
        case VK_SNAPSHOT:
            return ImGuiKey_PrintScreen;
        case VK_PAUSE:
            return ImGuiKey_Pause;
        case VK_NUMPAD0:
            return ImGuiKey_Keypad0;
        case VK_NUMPAD1:
            return ImGuiKey_Keypad1;
        case VK_NUMPAD2:
            return ImGuiKey_Keypad2;
        case VK_NUMPAD3:
            return ImGuiKey_Keypad3;
        case VK_NUMPAD4:
            return ImGuiKey_Keypad4;
        case VK_NUMPAD5:
            return ImGuiKey_Keypad5;
        case VK_NUMPAD6:
            return ImGuiKey_Keypad6;
        case VK_NUMPAD7:
            return ImGuiKey_Keypad7;
        case VK_NUMPAD8:
            return ImGuiKey_Keypad8;
        case VK_NUMPAD9:
            return ImGuiKey_Keypad9;
        case VK_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case VK_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case VK_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case VK_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case VK_ADD:
            return ImGuiKey_KeypadAdd;
        case IM_VK_KEYPAD_ENTER:
            return ImGuiKey_KeypadEnter;
        case VK_LSHIFT:
            return ImGuiKey_LeftShift;
        case VK_LCONTROL:
            return ImGuiKey_LeftCtrl;
        case VK_LMENU:
            return ImGuiKey_LeftAlt;
        case VK_LWIN:
            return ImGuiKey_LeftSuper;
        case VK_RSHIFT:
            return ImGuiKey_RightShift;
        case VK_RCONTROL:
            return ImGuiKey_RightCtrl;
        case VK_RMENU:
            return ImGuiKey_RightAlt;
        case VK_RWIN:
            return ImGuiKey_RightSuper;
        case VK_APPS:
            return ImGuiKey_Menu;
        case '0':
            return ImGuiKey_0;
        case '1':
            return ImGuiKey_1;
        case '2':
            return ImGuiKey_2;
        case '3':
            return ImGuiKey_3;
        case '4':
            return ImGuiKey_4;
        case '5':
            return ImGuiKey_5;
        case '6':
            return ImGuiKey_6;
        case '7':
            return ImGuiKey_7;
        case '8':
            return ImGuiKey_8;
        case '9':
            return ImGuiKey_9;
        case 'A':
            return ImGuiKey_A;
        case 'B':
            return ImGuiKey_B;
        case 'C':
            return ImGuiKey_C;
        case 'D':
            return ImGuiKey_D;
        case 'E':
            return ImGuiKey_E;
        case 'F':
            return ImGuiKey_F;
        case 'G':
            return ImGuiKey_G;
        case 'H':
            return ImGuiKey_H;
        case 'I':
            return ImGuiKey_I;
        case 'J':
            return ImGuiKey_J;
        case 'K':
            return ImGuiKey_K;
        case 'L':
            return ImGuiKey_L;
        case 'M':
            return ImGuiKey_M;
        case 'N':
            return ImGuiKey_N;
        case 'O':
            return ImGuiKey_O;
        case 'P':
            return ImGuiKey_P;
        case 'Q':
            return ImGuiKey_Q;
        case 'R':
            return ImGuiKey_R;
        case 'S':
            return ImGuiKey_S;
        case 'T':
            return ImGuiKey_T;
        case 'U':
            return ImGuiKey_U;
        case 'V':
            return ImGuiKey_V;
        case 'W':
            return ImGuiKey_W;
        case 'X':
            return ImGuiKey_X;
        case 'Y':
            return ImGuiKey_Y;
        case 'Z':
            return ImGuiKey_Z;
        case VK_F1:
            return ImGuiKey_F1;
        case VK_F2:
            return ImGuiKey_F2;
        case VK_F3:
            return ImGuiKey_F3;
        case VK_F4:
            return ImGuiKey_F4;
        case VK_F5:
            return ImGuiKey_F5;
        case VK_F6:
            return ImGuiKey_F6;
        case VK_F7:
            return ImGuiKey_F7;
        case VK_F8:
            return ImGuiKey_F8;
        case VK_F9:
            return ImGuiKey_F9;
        case VK_F10:
            return ImGuiKey_F10;
        case VK_F11:
            return ImGuiKey_F11;
        case VK_F12:
            return ImGuiKey_F12;
        default:
            return ImGuiKey_None;
    }
}

static void ImGui_ImplWin32_AddKeyEvent(ImGuiKey key, bool down, int native_keycode, int native_scancode = -1) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(key, down);
    io.SetKeyEventNativeData(key, native_keycode, native_scancode); // To support legacy indexing (<1.87 user code)
    IM_UNUSED(native_scancode);
}

void handle_mouse(HWND hwnd, ImGuiIO& io, ImGui_ImplWin32_Data* bd) {
    std::unique_lock lock_guard(input::mouse_mutex());

    auto& mouse_inputs = input::mouse_inputs();

    while (!mouse_inputs.empty()) {
        const auto& input = mouse_inputs.front();

        const bool button_down = input.flags & INPUT_MOUSE_FLAG_DOWN;
        switch (input.code) {
            case WM_MOUSEMOVE:
                // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
                bd->MouseHwnd = hwnd;
                if (!bd->MouseTracked)
                    bd->MouseTracked = true;

                io.AddMousePosEvent((float) input.x, (float) input.y);
                break;

            case VK_LBUTTON:
            case VK_RBUTTON:
            case VK_MBUTTON:
            case VK_XBUTTON1:
            case VK_XBUTTON2: {
                int button = 0;
                if (input.code == VK_LBUTTON) button = 0;
                if (input.code == VK_RBUTTON) button = 1;
                if (input.code == VK_MBUTTON) button = 2;
                if (input.code == VK_XBUTTON1) button = 3;
                if (input.code == VK_XBUTTON2) button = 4;

                if (button_down) {
                    bd->MouseButtonsDown |= 1 << button;
                    io.AddMouseButtonEvent(button, true);
                } else {
                    bd->MouseButtonsDown &= ~(1 << button);
                    io.AddMouseButtonEvent(button, false);
                }

                break;
            }

            case WM_MOUSEWHEEL:
                io.AddMouseWheelEvent((float) input.delta / (float) WHEEL_DELTA, 0.0f);
                break;
        }

        mouse_inputs.pop_front();
    }
}

void handle_keyboard(HWND hwnd, ImGuiIO& io, ImGui_ImplWin32_Data* bd) {
    const auto shift_key = [](wchar_t c) -> wchar_t {
        int16_t scan = VkKeyScanW(c);
        if (scan == -1)
            return -1;

        const uint32_t code = scan & 0xFF;

        uint8_t table[255] {};
        table[0x10] = 0x80;

        wchar_t result;
        if (ToUnicode(code, code, table, &result, 1, 0) != 1)
            return -1;

        return result;
    };

    std::unique_lock lock_guard(input::key_mutex());

    auto& key_inputs = input::key_inputs();

    while (!key_inputs.empty()) {
        const auto& input = key_inputs.front();

        if (input.flags & INPUT_KEY_FLAG_DOWN | input.flags & INPUT_KEY_FLAG_UP) {
            const bool down = (input.flags & INPUT_KEY_FLAG_DOWN) != 0;

            const ImGuiKey key = ImGui_ImplWin32_VirtualKeyToImGuiKey(input.code);
            if (key != ImGuiKey_None)
                ImGui_ImplWin32_AddKeyEvent(key, down, input.code, input.scan);

            if (input.code < 256)
                io.AddKeyEvent(key, down);

            if (down) {
                // WM_CHAR
                UINT key = MapVirtualKeyW(input.code, MAPVK_VK_TO_CHAR);
                if (key > 0) {
                    wchar_t key_char = key;

                    bool upper = false;
                    if ((input.flags & INPUT_KEY_FLAG_CAPS) != 0)
                        upper = !upper;

                    bool shift = false;
                    if (((input.flags & INPUT_KEY_FLAG_LSHIFT) | (input.flags & INPUT_KEY_FLAG_RSHIFT)) != 0) {
                        upper = !upper;
                        shift = true;
                    }

                    if (!shift) {
                        if (upper)
                            CharUpperW(&key_char);
                        else
                            CharLowerW(&key_char);
                    } else {
                        const auto shifted = shift_key(key_char);
                        if (shifted != (wchar_t) -1)
                            key_char = shifted;
                    }

                    io.AddInputCharacterUTF16(key_char);
                }
            }
        }

        key_inputs.pop_front();
    }
}

bool ImGui_ImplWin32_WndProcHandler_Hook(void* hwnd) {
    if (ImGui::GetCurrentContext() == nullptr)
        return false;

    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendPlatformUserData == nullptr)
        return false;

    ImGui_ImplWin32_Data* bd = (ImGui_ImplWin32_Data*) io.BackendPlatformUserData;

    handle_mouse((HWND) hwnd, io, bd);
    handle_keyboard((HWND) hwnd, io, bd);

    return true;
}
