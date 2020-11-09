#include "Draw.hpp"

using namespace std;

#define FWIDTH int(floor((render_state.width - render_state.right_padding)/game_data.N))
#define FHEIGHT int(floor((render_state.height- render_state.bottom_padding)/game_data.M))

#define NEXT_PAGE 1
#define PREV_PAGE 2

Render_State render_state; // muszáj hogy globális legyen, mert csak a WndProc-on belül érhető el a méret módosítás, és az pedig static
Game_Data game_data;
std::vector<std::vector<std::vector<std::vector<std::string>>>> fd;
WNDCLASS window_class;
HWND main_window;
HWND disp_tick;
HWND disp_info;
HWND next_button;
HWND prev_button;
vector<vector<HWND>> field_btns;
HDC hdc;

void refreshWindowElements() {
    for (int i = 0; i < game_data.N; i++ ) {
        for(int j = 0; j < game_data.M; j++) {
            SetWindowPos(field_btns[i][j], HWND_TOP,  i*FWIDTH, j*FHEIGHT, FWIDTH, FHEIGHT, 0);
            SetWindowTextA(field_btns[i][j], "");
        }
    }
    SetWindowTextA(next_button, ">");
    SetWindowTextA(prev_button, "<");
}

void nextPage(HWND hwnd){
    if (game_data.tick < game_data.maxtick) {
        refresh();
        game_data.tick++;
        if (!game_data.fd.empty()){
            for (int i = 0; i < game_data.N; i++) {
                for (int j = 0; j < game_data.M; j++) {
                    string t;
                    for (const auto& m : game_data.fd[game_data.tick][i][j]) {
                        t += m + "\n";
                    }
                    SetWindowTextA(field_btns[i][j], t.c_str());
                }
            }
        }
        string t;
        stringstream ss;
        ss << game_data.tick;
        ss >> t;
        t = "tick=" + t;

        SetWindowTextA(disp_tick, t.c_str());
    }
}

void prevPage(HWND hwnd) {
    if (game_data.tick > 0) {
        game_data.tick--;
        for (int i = 0; i < game_data.N; i++) {
            for (int j = 0; j < game_data.M; j++) {

            }
        }
        string t;
        stringstream ss;
        ss << game_data.tick;
        ss >> t;
        t = "tick=" + t;
        refresh();
        SetWindowTextA(disp_tick, t.c_str());
    }
}

void addControls(HWND hwnd) {
    // SS_LEFT = static stile align left
    disp_info = CreateWindowA("static", "text", WS_CHILD | WS_BORDER | SS_LEFT, 0, 0, 50, 50, hwnd, nullptr, nullptr, nullptr);
    disp_tick = CreateWindowA("static", "tick = 0", WS_VISIBLE | WS_CHILD | SS_CENTER, 0, 0, 50, 50, hwnd, nullptr, nullptr, nullptr);
    next_button = CreateWindowA("button", ">", WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hwnd, (HMENU)NEXT_PAGE, nullptr, nullptr);
    prev_button = CreateWindowA("button", "<", WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hwnd, (HMENU)PREV_PAGE, nullptr, nullptr);
}

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
    clear();
    SetWindowPos(next_button, nullptr, render_state.width-render_state.right_padding-70, render_state.height-render_state.bottom_padding+20, 70, 50, 0);
    SetWindowPos(prev_button, nullptr, render_state.width-render_state.right_padding-70-70, render_state.height-render_state.bottom_padding+20, 70, 50, 0);
    SetWindowPos(disp_tick, nullptr, render_state.width-render_state.right_padding-70-70-70-10, render_state.height-render_state.bottom_padding+20, 70, 50, 0);
    refresh();
}

LRESULT CALLBACK Draw::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_COMMAND:
            switch(wParam) {
                case NEXT_PAGE:{
                    nextPage(hwnd);
                } break;
                case PREV_PAGE: {
                    prevPage(hwnd);
                }break;
            }break;
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_CREATE:
            addControls(hwnd);
        case WM_SIZE:
            setWindowSize(hwnd);
            break;
        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}

void Draw::draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_info) {
    if (field_info.empty()) {
        return;
    }
    fd = field_info;
    game_data.tick = 0;
    game_data.maxtick = fd.size();
    game_data.N = fd[0][0].size();
    game_data.M = fd[0].size();
    render_state.right_padding = 100;
    render_state.bottom_padding = 100;
    field_btns.resize(game_data.N, vector<HWND> (game_data.M));

    for (int i = 0; i < game_data.N; i++ ) {
        for(int j = 0; j < game_data.M; j++) {
            field_btns[i][j] = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, i*FWIDTH, j*FHEIGHT, FWIDTH, FHEIGHT, main_window, nullptr, nullptr, nullptr);
        }
    }

    SendMessageA(main_window, WM_SIZE, 0, 0);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
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
    hdc = GetDC(main_window);
}




