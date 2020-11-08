#include "Draw.hpp"

using namespace std;

Render_State render_state; // muszáj hogy globális legyen, mert csak a WndProc-on belül érhető el a méret módosítás, és az pedig static
WNDCLASS window_class;
HWND main_window;
HDC hdc;

void setWindowSize(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    render_state.width = rect.right - rect.left;
    render_state.height = rect.bottom - rect.top;

    uint32_t size = render_state.width * render_state.height * sizeof(unsigned int);

    if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
    render_state.memory = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
    render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
    render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
    render_state.bitmap_info.bmiHeader.biPlanes = 1;
    render_state.bitmap_info.bmiHeader.biBitCount = 32;
    render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
}

LRESULT CALLBACK Draw::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_SIZE:
            setWindowSize(hwnd);
            break;
        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}

void Draw::draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& fd) {
    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        draw_filled_rect(0, 0, 40, 40, 0x00ff00);
        refresh();

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


Draw::Draw(HINSTANCE hInstance_) {
    // Create Window Class
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "Game Window Class";
    window_class.lpfnWndProc = WndProc;

    // Register Class
    RegisterClass(&window_class);

    // Create Window
    main_window = CreateWindow(window_class.lpszClassName, "Unagi", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, hInstance_, nullptr);

    // hdc = device content handler
    hdc = GetDC(main_window);
}




