#include <window.h>

int main() {
    sr_window_t window;

    sr_window_create(&window, 600, 600);
    sr_window_loop(&window);
    sr_window_destroy(&window);

    return SR_SUCCESS_EXIT_CODE;
}
