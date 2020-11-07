#include "Draw.hpp"

using namespace std;

static bool running = true;

struct Render_State {
    int height, width;
    void* memory; // void pointer: nem érdekel minket milyen típusú

    BITMAPINFO bitmap_info;
};

static Render_State render_state;

LRESULT CALLBACK Draw::window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
        } break;
        case WM_SIZE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            render_state.width = rect.right - rect.left;
            render_state.height = rect.bottom - rect.top;

            uint32_t size = render_state.width * render_state.height * sizeof(unsigned int);

            if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
            render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
            render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
            render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
            render_state.bitmap_info.bmiHeader.biPlanes = 1;
            render_state.bitmap_info.bmiHeader.biBitCount = 32;
            render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
        } break;
        default:
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return result;
}

void Draw::draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& fd) {
    while (running) {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        string txt = "This is how to create text";
        TextOut(hdc, 10, 50, txt.c_str(), txt.size());

    }
}

Draw::Draw(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd) : hInstance(_hInstance), hPrevInstance(_hPrevInstance), lpCmdLine(_lpCmdLine), nShowCmd(_nShowCmd) {
    ShowCursor(TRUE);

    // Create Window class
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "Game Window Class";
    window_class.lpfnWndProc = window_callback;

    // Register class
    RegisterClass(&window_class);

    // Create window
    window = CreateWindowA(window_class.lpszClassName, "Ericsson2020 Unagi", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);

    hdc = GetDC(window); // get device context
}

void Draw::refresh() {
    StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}

void Draw::clear() {
    auto* pixel = (unsigned int*)render_state.memory;
    for (int y = 0; y < render_state.height; ++y) {
        for (int x = 0; x < render_state.width; ++x) {
            *pixel++ = 0xffffff;
        }
    }
}

void Draw::draw_filled_rect(int x0, int y0, int width, int height, uint32_t color) {
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

void Draw::draw_rect(int x0, int y0, int width, int height, uint32_t color, int border_width) {
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

uint32_t Draw::coord(int x, int y) {
    return x + y*render_state.width;;
}



