#pragma once

#include <renderer.h>

#if defined(SR_WINDOW_USE_XCB)
    #include <xcb/xcb_image.h>

    typedef struct {
        sr_renderer_t        sr_renderer;
        xcb_connection_t   * xcb_connection;
        xcb_gcontext_t       xcb_gcontext;
        xcb_window_t         xcb_window;
        xcb_atom_t           xcb_atom_delete_window;
        uint8_t              xcb_screen_depth;
    } sr_window_t;
#elif defined(SR_WINDOW_USE_WAYLAND)
    #include <wayland-client.h>
#endif

void sr_window_create(register sr_window_t * restrict  window, register const uint16_t width, register const uint16_t height);
void sr_window_loop(register sr_window_t * restrict window);
void sr_window_destroy(register sr_window_t * restrict window);
