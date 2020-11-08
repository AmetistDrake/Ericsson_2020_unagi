#include "Draw.hpp"

using namespace std;

void draw_filled_rect(int x0, int y0, int width, int height, uint32_t color) {
    x0 = clamp(x0, 0, render_state.width);
    y0 = clamp(y0, 0, render_state.height);
    int x1 = clamp(x0 + width, 0, render_state.width);
    int y1 = clamp(y0 + height, 0, render_state.height);
    for (int y = y0; y < y1; y++) {
        uint32_t* pixel = (uint32_t *)render_state.memory + x0 + y*render_state.width;
        for (int x = x0; x < x1; x++) {
            *pixel++ = color;
        }
    }
}

void refresh() {
    StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}