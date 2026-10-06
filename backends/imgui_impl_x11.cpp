#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_x11.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <time.h>
#if not defined IMGUI_IMPL_X11_NO_XFIXES
#include <X11/extensions/Xfixes.h>
#endif
#include <stdio.h>


struct ImGui_ImplX11_Data
{
    Display*                    display;
	Window                      window;
    ImGuiMouseCursor            last_cursor;
    XIM                         xim;
    XIC                         xic;
    double                      time;
	ImVec2                      window_size;
	ImVec2                      window_pos;
    bool                        mouse_outside_window;

    ImGui_ImplX11_Data()		{ ::memset((void*)this, 0, sizeof(*this)); }
};

static ImGui_ImplX11_Data* ImGui_ImplX11_GetBackendData( )
{
    return ImGui::GetCurrentContext( ) ? reinterpret_cast< ImGui_ImplX11_Data* >( ImGui::GetIO( ).BackendPlatformUserData ) : nullptr;
}

static ImGuiKey ImGui_ImplX11_KeyCode_to_ImKey( KeySym key )
{
    switch( key )
    {
        // Letters
        case XK_a: return ImGuiKey_A;
        case XK_b: return ImGuiKey_B;
        case XK_c: return ImGuiKey_C;
        case XK_d: return ImGuiKey_D;
        case XK_e: return ImGuiKey_E;
        case XK_f: return ImGuiKey_F;
        case XK_g: return ImGuiKey_G;
        case XK_h: return ImGuiKey_H;
        case XK_i: return ImGuiKey_I;
        case XK_j: return ImGuiKey_J;
        case XK_k: return ImGuiKey_K;
        case XK_l: return ImGuiKey_L;
        case XK_m: return ImGuiKey_M;
        case XK_n: return ImGuiKey_N;
        case XK_o: return ImGuiKey_O;
        case XK_p: return ImGuiKey_P;
        case XK_q: return ImGuiKey_Q;
        case XK_r: return ImGuiKey_R;
        case XK_s: return ImGuiKey_S;
        case XK_t: return ImGuiKey_T;
        case XK_u: return ImGuiKey_U;
        case XK_v: return ImGuiKey_V;
        case XK_w: return ImGuiKey_W;
        case XK_x: return ImGuiKey_X;
        case XK_y: return ImGuiKey_Y;
        case XK_z: return ImGuiKey_Z;

        // Digits
        case XK_0: return ImGuiKey_0;
        case XK_1: return ImGuiKey_1;
        case XK_2: return ImGuiKey_2;
        case XK_3: return ImGuiKey_3;
        case XK_4: return ImGuiKey_4;
        case XK_5: return ImGuiKey_5;
        case XK_6: return ImGuiKey_6;
        case XK_7: return ImGuiKey_7;
        case XK_8: return ImGuiKey_8;
        case XK_9: return ImGuiKey_9;

        // Function keys
        case XK_F1:  return ImGuiKey_F1;
        case XK_F2:  return ImGuiKey_F2;
        case XK_F3:  return ImGuiKey_F3;
        case XK_F4:  return ImGuiKey_F4;
        case XK_F5:  return ImGuiKey_F5;
        case XK_F6:  return ImGuiKey_F6;
        case XK_F7:  return ImGuiKey_F7;
        case XK_F8:  return ImGuiKey_F8;
        case XK_F9:  return ImGuiKey_F9;
        case XK_F10: return ImGuiKey_F10;
        case XK_F11: return ImGuiKey_F11;
        case XK_F12: return ImGuiKey_F12;

        // Arrow keys
        case XK_Left:  return ImGuiKey_LeftArrow;
        case XK_Right: return ImGuiKey_RightArrow;
        case XK_Up:    return ImGuiKey_UpArrow;
        case XK_Down:  return ImGuiKey_DownArrow;

        // Navigation/editing
        case XK_Home:     return ImGuiKey_Home;
        case XK_End:      return ImGuiKey_End;
        case XK_Insert:   return ImGuiKey_Insert;
        case XK_Delete:   return ImGuiKey_Delete;
        case XK_Page_Up:  return ImGuiKey_PageUp;
        case XK_Page_Down:return ImGuiKey_PageDown;
        case XK_BackSpace:return ImGuiKey_Backspace;
        case XK_Return:   return ImGuiKey_Enter;
        case XK_Escape:   return ImGuiKey_Escape;
        case XK_Tab:      return ImGuiKey_Tab;
        case XK_space:    return ImGuiKey_Space;
        case XK_KP_Enter: return ImGuiKey_KeypadEnter;

        // Modifiers
        case XK_Shift_L:    return ImGuiKey_LeftShift;
        case XK_Shift_R:    return ImGuiKey_RightShift;
        case XK_Control_L:  return ImGuiKey_LeftCtrl;
        case XK_Control_R:  return ImGuiKey_RightCtrl;
        case XK_Alt_L:      return ImGuiKey_LeftAlt;
        case XK_Alt_R:      return ImGuiKey_RightAlt;
        case XK_Meta_L:     return ImGuiKey_LeftSuper;
        case XK_Meta_R:     return ImGuiKey_RightSuper;

        // Numpad keys
        case XK_KP_0: return ImGuiKey_Keypad0;
        case XK_KP_1: return ImGuiKey_Keypad1;
        case XK_KP_2: return ImGuiKey_Keypad2;
        case XK_KP_3: return ImGuiKey_Keypad3;
        case XK_KP_4: return ImGuiKey_Keypad4;
        case XK_KP_5: return ImGuiKey_Keypad5;
        case XK_KP_6: return ImGuiKey_Keypad6;
        case XK_KP_7: return ImGuiKey_Keypad7;
        case XK_KP_8: return ImGuiKey_Keypad8;
        case XK_KP_9: return ImGuiKey_Keypad9;
        case XK_KP_Add:       return ImGuiKey_KeypadAdd;
        case XK_KP_Subtract:  return ImGuiKey_KeypadSubtract;
        case XK_KP_Multiply:  return ImGuiKey_KeypadMultiply;
        case XK_KP_Divide:    return ImGuiKey_KeypadDivide;
        case XK_KP_Decimal:   return ImGuiKey_KeypadDecimal;

        // Symbols
        case XK_equal:      return ImGuiKey_Equal;
        case XK_minus:      return ImGuiKey_Minus;
        case XK_grave:      return ImGuiKey_GraveAccent;
        case XK_apostrophe: return ImGuiKey_Apostrophe;
        case XK_semicolon:  return ImGuiKey_Semicolon;
        case XK_comma:      return ImGuiKey_Comma;
        case XK_period:     return ImGuiKey_Period;
        case XK_slash:      return ImGuiKey_Slash;
        case XK_backslash:  return ImGuiKey_Backslash;
        case XK_bracketleft: return ImGuiKey_LeftBracket;
        case XK_bracketright:return ImGuiKey_RightBracket;

        default:
            return ImGuiKey_None;
    }
}

IMGUI_IMPL_API void ImGui_ImplX11_KeyEvent( XEvent* p_event, bool down )
{
    ImGuiIO& io     = ImGui::GetIO( );
    KeySym keysym   = XLookupKeysym( reinterpret_cast< XKeyEvent* >( p_event ), 0 );
    ImGuiKey key    = ImGui_ImplX11_KeyCode_to_ImKey( keysym );
    
    if ( key != ImGuiKey_None )
    {
        io.AddKeyEvent( key, down );
        io.SetKeyEventNativeData( key, p_event->xkey.keycode, down ); // To support legacy indexing (<1.87 user code)
    }

    if ( down )
    {
        ImGui_ImplX11_Data* bd          = ImGui_ImplX11_GetBackendData( );
        XKeyPressedEvent* pressed_event = reinterpret_cast< XKeyPressedEvent* >( p_event );

        char buf[ 64 ] = { };
        KeySym keysym;
        Status status;
        
        int len = Xutf8LookupString( bd->xic, pressed_event, buf, sizeof( buf ), &keysym, &status );

        if ( status == XLookupChars || status == XLookupBoth )
            io.AddInputCharactersUTF8( buf );
    }
}

IMGUI_IMPL_API void ImGui_ImplX11_MouseEvent( XEvent* p_event, bool down )
{
    ImGuiIO& io             = ImGui::GetIO();
    ImGuiMouseButton curbtn = 0;
    int mode                = 0;
    float wheelx            = 0.f;
    float wheely            = 0.f;

    switch( p_event->xbutton.button )
    {
        case 1:   mode = 1;  curbtn = ImGuiMouseButton_Left;    break;
        case 2:   mode = 1;  curbtn = ImGuiMouseButton_Middle;  break;
        case 3:   mode = 1;  curbtn = ImGuiMouseButton_Right;   break;
        case 4:   mode = 2;  wheely  = 1.f;                     break;
        case 5:   mode = 2;  wheely  = -1.f;                    break;
        case 6:   mode = 2;  wheelx  = 1.f;                     break;
        case 7:   mode = 2;  wheelx  = -1.f;                    break;
        case 8:   mode = 1;  curbtn = ImGuiMouseButton_X1;      break;
        case 9:   mode = 1;  curbtn = ImGuiMouseButton_X2;      break;
        default:  return;
    }
    
    switch ( mode ) 
    {
        case 1:   io.AddMouseButtonEvent( curbtn, down );   break;
        case 2:   io.AddMouseWheelEvent( wheelx, wheely );  break;
        default:  return;
    }
}

IMGUI_IMPL_API bool ImGui_ImplX11_OnEvent(void* event)
{
    if( !ImGui::GetCurrentContext( ) )
        return true;

    XEvent* evt = reinterpret_cast< XEvent* >( event );
    ImGuiIO& io = ImGui::GetIO( );
    ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData( );

    switch( evt->type )
    {
        case KeyPress:
            ImGui_ImplX11_KeyEvent( evt, true );
            break;
        case KeyRelease:
            ImGui_ImplX11_KeyEvent( evt, false );
            break;
        case ButtonPress:
            ImGui_ImplX11_MouseEvent( evt, true );
            break;
        case ButtonRelease:
            ImGui_ImplX11_MouseEvent( evt, false );
            break;
        /*case MotionNotify:
            io.AddMousePosEvent( static_cast< float >( evt->xmotion.x ), static_cast< float >( evt->xmotion.y ) );
            break; <- for some reason, this is lagging behind A LOT, dont use it*/
        default:
            break;
    }

    return true;
}

IMGUI_IMPL_API bool ImGui_ImplX11_UpdateMouseCursor(ImGui_ImplX11_Data* bd)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        #if not defined IMGUI_IMPL_X11_NO_XFIXES
        XFixesHideCursor(bd->display, bd->window);
        #endif
        //XDefineCursor(bd->display, bd->window, XCreateFontCursor(bd->display, -1)); /* Not working unfortunately */
    }
    else
    {
        // Show OS mouse cursor
        unsigned int shape;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        shape = XC_arrow;               break;
        case ImGuiMouseCursor_TextInput:    shape = XC_xterm;               break;
        case ImGuiMouseCursor_ResizeAll:    shape = XC_fleur;               break;
        case ImGuiMouseCursor_ResizeEW:     shape = XC_sb_h_double_arrow;   break;
        case ImGuiMouseCursor_ResizeNS:     shape = XC_sb_v_double_arrow;   break;
        case ImGuiMouseCursor_ResizeNESW:   shape = XC_bottom_right_corner; break;
        case ImGuiMouseCursor_ResizeNWSE:   shape = XC_bottom_left_corner;  break;
        case ImGuiMouseCursor_Hand:         shape = XC_hand1;               break;
        case ImGuiMouseCursor_NotAllowed:   shape = XC_X_cursor;            break;
        }

        #if not defined IMGUI_IMPL_X11_NO_XFIXES
        XFixesShowCursor(bd->display, bd->window);
        #endif
        XDefineCursor(bd->display, bd->window, XCreateFontCursor(bd->display, shape));
    }
    return true;
}

IMGUI_IMPL_API bool ImGui_ImplX11_Init(void* display, int window)
{
	ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

	ImGui_ImplX11_Data* bd = IM_NEW(ImGui_ImplX11_Data)();

    bd->display = reinterpret_cast< Display* >(display);
    bd->window  = static_cast< Window >(window);
    
    bd->xim = XOpenIM(bd->display, NULL, NULL, NULL);

    bd->time = 0.0;
    
    IM_ASSERT( bd->xim != nullptr && "XOpenIM seems to have failed!" );
    
    bd->xic = XCreateIC
    (
        bd->xim,
        XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow, bd->window,
        XNFocusWindow, bd->window,
        nullptr
    );

    IM_ASSERT( bd->xic != nullptr && "XCreateIC seems to have failed!" );
    
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_x11";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; 

	return true;
}

IMGUI_IMPL_API void ImGui_ImplX11_Shutdown()
{
	ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();
      
    if ( bd->xic )
    {
        XDestroyIC( bd->xic );
        bd->xic = nullptr;
    }

    if ( bd->xim )
    {
        XCloseIM( bd->xim );
        bd->xim = nullptr;
    }

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors);
    IM_DELETE(bd);
}

IMGUI_IMPL_API void ImGui_ImplX11_NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplX11_Init()?");

    // Setup time step
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double now = ts.tv_sec + ts.tv_nsec / 1e9;
    io.DeltaTime = (float)(now - bd->time);
    bd->time = now;

    XWindowAttributes attr ={};
    XGetWindowAttributes(bd->display, bd->window, &attr);

    io.DisplaySize.x = static_cast<float>(attr.width);
    io.DisplaySize.y = static_cast<float>(attr.height);

    Window ptrWndR, ptrWndC     = 0;
    int x_root, y_root, x, y    = 0;
    unsigned int mask           = 0;
    XQueryPointer(bd->display, bd->window, &ptrWndR, &ptrWndC, &x_root, &y_root, &x, &y, &mask);

    int abs_x, abs_y;
    Window child;
    XTranslateCoordinates(bd->display, bd->window, DefaultRootWindow(bd->display), 0, 0, &abs_x, &abs_y, &child);

    bd->window_pos  = ImVec2( static_cast< float >( attr.x ), static_cast< float >( attr.y ) );
    bd->window_size = ImVec2( static_cast< float >( attr.width ), static_cast< float >( attr.height ) );
    
    if ( io.MousePos.x != x || io.MousePos.y != y )
        io.AddMousePosEvent( static_cast< float >( x ), static_cast< float >( y ) );
    
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (bd->last_cursor != mouse_cursor)
    {
        bd->last_cursor = mouse_cursor;
        ImGui_ImplX11_UpdateMouseCursor(bd);
    }

    auto cur_outside = ( x_root < abs_x || y_root < abs_y || x_root > abs_x + attr.width || y_root > abs_y + attr.height );

    if ( bd->mouse_outside_window != cur_outside )
    {
        bd->mouse_outside_window = cur_outside;

        #if not defined IMGUI_IMPL_X11_NO_XFIXES
        if (cur_outside)
            XFixesShowCursor(bd->display, bd->window);
        else if (io.MouseDrawCursor)
            XFixesHideCursor(bd->display, bd->window);
        
        #endif
    }

}


#endif // #ifndef IMGUI_DISABLE
