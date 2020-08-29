#include <window.h>

#ifndef SR_WINDOW_TITLE
    #define SR_WINDOW_TITLE "SoftRays"
#endif

#if defined(SR_WINDOW_USE_XCB)
    typedef union {
        xcb_generic_event_t * restrict xcb_generic;
        xcb_client_message_event_t * restrict xcb_message;
        xcb_configure_notify_event_t * restrict xcb_notify;
    } sr_window_event_t;

    SR_INLINE void sr_window_create_xcb(register sr_window_t * restrict window, register const uint32_t width,
                                        register const uint32_t height) {
        register xcb_screen_t * restrict xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(window->xcb_connection)).data;

        window->xcb_window = xcb_generate_id(window->xcb_connection);
        xcb_create_window(window->xcb_connection, xcb_screen->root_depth, window->xcb_window, xcb_screen->root,
                          0, 0, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual,
                          XCB_CW_EVENT_MASK, (const uint32_t []){ XCB_EVENT_MASK_STRUCTURE_NOTIFY });

        window->xcb_screen_depth = xcb_screen->root_depth;
    }

    SR_INLINE void sr_window_get_delete_atom(register sr_window_t * restrict window) {
        static const char wm_protocols[] = "WM_PROTOCOLS";
        register xcb_intern_atom_reply_t * restrict protocols_reply = xcb_intern_atom_reply(window->xcb_connection,
            xcb_intern_atom(window->xcb_connection, 1,sizeof(wm_protocols) - 1, wm_protocols), NULL);

        static const char wm_delete_window[] = "WM_DELETE_WINDOW";
        register xcb_intern_atom_reply_t * restrict delete_window_reply = xcb_intern_atom_reply(window->xcb_connection,
            xcb_intern_atom(window->xcb_connection, 0, sizeof(wm_delete_window) - 1, wm_delete_window), NULL);

        xcb_change_property(window->xcb_connection, XCB_PROP_MODE_REPLACE, window->xcb_window, protocols_reply->atom,
                            XCB_ATOM_ATOM, sizeof(xcb_atom_t) * 8, 1, &delete_window_reply->atom);

        window->xcb_atom_delete_window = delete_window_reply->atom;
    }

    inline void sr_window_create(register sr_window_t * restrict window, register const uint16_t width,
                                 register const uint16_t height) {
        window->xcb_connection = xcb_connect(NULL, NULL);

        if (window->xcb_connection == NULL) {
            sr_exit(SR_WINDOW_XCB_CONNECT_ERROR_CODE);
        }

        sr_window_create_xcb(window, width, height);

        window->xcb_gcontext = xcb_generate_id(window->xcb_connection);
        xcb_create_gc(window->xcb_connection, window->xcb_gcontext, window->xcb_window, 0, NULL);

        xcb_change_property(window->xcb_connection, XCB_PROP_MODE_REPLACE, window->xcb_window, XCB_ATOM_WM_NAME,
                            XCB_ATOM_STRING, 8, sizeof(SR_WINDOW_TITLE) - 1, SR_WINDOW_TITLE);

        sr_window_get_delete_atom(window);

        xcb_map_window(window->xcb_connection, window->xcb_window);
        xcb_flush(window->xcb_connection);

        sr_renderer_create(&window->sr_renderer, width, height);
        window->sr_renderer.pixels = malloc(window->sr_renderer.pixels_size);
    }

    inline void sr_window_loop(register sr_window_t * restrict window) {
        for (sr_window_event_t window_event; SR_TRUE; xcb_flush(window->xcb_connection)) {
            window_event.xcb_generic = xcb_poll_for_event(window->xcb_connection);

            if (window_event.xcb_generic != NULL) {
                switch (window_event.xcb_generic->response_type & ~0x80) {
                    case XCB_CLIENT_MESSAGE: {
                        if (window_event.xcb_message->data.data32[0] == window->xcb_atom_delete_window) {
                            free(window_event.xcb_generic);
                            return;
                        }
                    } break;
                    case XCB_CONFIGURE_NOTIFY: {
                        if (window->sr_renderer.width != window_event.xcb_notify->width ||
                            window->sr_renderer.height != window_event.xcb_notify->height) {
                                sr_renderer_set_sizes(&window->sr_renderer, window_event.xcb_notify->width,
                                                      window_event.xcb_notify->height);
                                window->sr_renderer.pixels = realloc(window->sr_renderer.pixels,
                                                                     window->sr_renderer.pixels_size);
                        }
                    } break;
                }

                free(window_event.xcb_generic);
            }

            sr_renderer_draw(&window->sr_renderer);

            register xcb_image_t * xcb_image = xcb_image_create_native(window->xcb_connection, window->sr_renderer.width,
                window->sr_renderer.height, XCB_IMAGE_FORMAT_Z_PIXMAP, window->xcb_screen_depth, NULL,
                window->sr_renderer.pixels_size, (void *)window->sr_renderer.pixels);

            xcb_image_put(window->xcb_connection, window->xcb_window, window->xcb_gcontext, xcb_image, 0, 0, 0);
            xcb_image_destroy(xcb_image);
        }
    }

    inline void sr_window_destroy(register sr_window_t * restrict window) {
        sr_renderer_destroy(&window->sr_renderer);
        free(window->sr_renderer.pixels);

        xcb_disconnect(window->xcb_connection);
    }
#else
    #ifndef SR_WINDOW_CLASS
        #define SR_WINDOW_CLASS "srClass"
    #endif

    static sr_window_t * sr_window;

    inline HBITMAP sr_create_pixel_buffer(register sr_pixel_t ** pixels, register const HDC hdc,
                                   register const uint16_t width, register const uint16_t height) {
        return CreateDIBSection(hdc, &(BITMAPINFO){
                                    .bmiColors[0] = { 0 },
                                    .bmiHeader = {
                                        .biSize             = sizeof(BITMAPINFOHEADER),
                                        .biWidth            = width,
                                        .biHeight           = -height,
                                        .biPlanes           = 1,
                                        .biBitCount         = 32,
                                        .biCompression      = BI_RGB,
                                        .biSizeImage        = 0,
                                        .biXPelsPerMeter    = 0,
                                        .biYPelsPerMeter    = 0,
                                        .biClrUsed          = 0,
                                        .biClrImportant     = 0,
                                    },
                                }, DIB_RGB_COLORS, (void **)(pixels), NULL, 0);
    }

    LRESULT CALLBACK sr_window_processor(register const HWND h_window, register const UINT message_id,
                                         register const WPARAM w_param, register const LPARAM l_param) {
        switch (message_id) {
            case WM_PAINT: {
                PAINTSTRUCT paint_struct;
                register const HDC window_hdc = BeginPaint(h_window, &paint_struct);
                register const HDC bitmap_hdc = CreateCompatibleDC(window_hdc);

                SelectObject(bitmap_hdc, sr_window->h_bitmap);
                BitBlt(window_hdc, 0, 0, sr_window->sr_renderer.width, sr_window->sr_renderer.height,
                       bitmap_hdc, 0, 0, SRCCOPY);

                DeleteDC(bitmap_hdc);
                EndPaint(h_window, &paint_struct);
            } return SR_FALSE;
            case WM_SIZE: {
                DeleteObject(sr_window->h_bitmap);
                sr_renderer_set_sizes(&sr_window->sr_renderer,
                    LOWORD(l_param),  HIWORD(l_param));
                sr_window->h_bitmap = sr_create_pixel_buffer(&sr_window->sr_renderer.pixels,
                    GetDC(sr_window->h_window), sr_window->sr_renderer.width, sr_window->sr_renderer.height);
            } return SR_TRUE;
            case WM_CLOSE: {
                sr_window->sr_continue_loop = SR_FALSE;
            } return SR_FALSE;
        }

        return DefWindowProc(h_window, message_id, w_param, l_param);
    }

    inline void sr_window_create(register sr_window_t * restrict window, register const uint16_t width,
                                 register const uint16_t height) {
        register const HINSTANCE h_instance = GetModuleHandle(NULL);
        sr_window = window;

        RegisterClassEx(&(WNDCLASSEX){
                            .cbSize         = sizeof(WNDCLASSEX),
                            .style          = 0,
                            .lpfnWndProc    = sr_window_processor,
                            .cbClsExtra     = 0,
                            .cbWndExtra     = 0,
                            .hInstance      = h_instance,
                            .hIcon          = NULL,
                            .hCursor        = LoadCursor(NULL, IDC_ARROW),
                            .hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1),
                            .lpszMenuName   = NULL,
                            .lpszClassName  = TEXT(SR_WINDOW_CLASS),
                            .hIconSm        = NULL,
                        });

        window->h_window = CreateWindowEx(WS_EX_CLIENTEDGE, SR_WINDOW_CLASS, SR_WINDOW_TITLE,
                                          WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                                          width, height, NULL, NULL, h_instance, NULL);

        if (window->h_window == NULL) {
            sr_exit(SR_WINDOW_WINAPI_CREATE_ERROR_CODE);
        }

        sr_renderer_create(&window->sr_renderer, width, height);
        window->h_bitmap = sr_create_pixel_buffer(&window->sr_renderer.pixels, GetDC(window->h_window),
                                                  window->sr_renderer.width, window->sr_renderer.height);
    }

    inline void sr_window_loop(register sr_window_t * restrict window) {
        MSG window_message;
        sr_window = window;

        for (window->sr_continue_loop = SR_TRUE; window->sr_continue_loop; /* void */) {
            if (PeekMessage(&window_message, NULL, 0, 0, PM_REMOVE)) {
                DispatchMessage(&window_message);
            } else {
                sr_renderer_draw(&window->sr_renderer);
                InvalidateRect(window_message.hwnd, NULL, FALSE);
            }
        }
    }

    inline void sr_window_destroy(register sr_window_t * restrict window) {
        sr_renderer_destroy(&window->sr_renderer);

        DestroyWindow(window->h_window);
        DeleteObject(window->h_bitmap);
    }
#endif
