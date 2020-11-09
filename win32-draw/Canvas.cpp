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

void draw_rect(int x0, int y0, int width, int height, uint32_t color, int border_width) {
    uint32_t* pixel;
    x0 = clamp(x0, 0, render_state.width);
    y0 = clamp(y0, 0, render_state.height);
    int x1 = clamp(x0 + width, 0, render_state.width);
    int y1 = clamp(y0 + height, 0, render_state.height);
    for (int y : {y0, y1-1}) {
        for (int k = 0; k < border_width; k++) {
            int z;
            if (y == y0) {
                z = clamp(y-k, 0, render_state.height);
            } else {
                z = clamp(y+k, 0, render_state.height);
            }
            pixel = (uint32_t *)render_state.memory + coord(x0, z); // vízszintes vonalak, y változik
            for (int x = x0; x < x1; x++) {
                *pixel++ = color;
            }
        }

    }

    for (int y = clamp(y0-border_width+1, 0, render_state.width); y < clamp(y1+border_width-1, 0, render_state.width); y++) {
        for (int x : {x0, x1-1}) {
            for (int k = 0; k < border_width; k++) {
                int z;
                if (x == x0) {
                    z = clamp(x0 - k, 0, render_state.width);
                }
                else {
                    z = clamp(x1 + k, 0, render_state.width);
                }

                pixel = (uint32_t *)render_state.memory + coord(z, y);
                *pixel = color;
            }
        }
    }
}

void refresh() {
    StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    refreshWindowElements();
}

void clear() {
    auto* pixel = (unsigned int*)render_state.memory;
    for (int y = 0; y < render_state.height; ++y) {
        for (int x = 0; x < render_state.width; ++x) {
            *pixel++ = 0xffffff;
        }
    }
}

uint32_t coord(int x, int y) {
    return x + y*render_state.width;
}