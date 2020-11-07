#include "Draw.hpp"

using namespace std;

bool running = true;

struct Render_State {
    int height, width;
    void* memory; // void pointer: nem érdekel minket milyen típusú

    BITMAPINFO bitmap_info;
};

Render_State render_state;

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

void Draw::draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_info) {
    ShowCursor(TRUE);

    // Create Window class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "Game Window Class";
    window_class.lpfnWndProc = window_callback;

    // Register class
    RegisterClass(&window_class);

    // Create window
    HWND window = CreateWindowA(window_class.lpszClassName, "Vírusirtó!", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
    {
        //Fullscreen
//        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
//        MONITORINFO mi = { sizeof(mi) };
//        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
//        SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    HDC hdc = GetDC(window); // get device context

    while (running) {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        // Simulate
        unsigned int* pixel = (unsigned int*)render_state.memory;
        for (int y = 0; y < render_state.height; ++y) {
            for (int x = 0; x < render_state.width; ++x) {
                *pixel++ = x*y;
            }
        }

        // Render
        StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    }
}

Draw::Draw(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd) : hInstance(_hInstance), hPrevInstance(_hPrevInstance), lpCmdLine(_lpCmdLine), nShowCmd(_nShowCmd) {}

