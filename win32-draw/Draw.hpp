#ifndef ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
#define ERICSSON_SOCKET_CONNECTOR_DRAW_HPP

#include <windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

class Draw {
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    Draw() = delete;
    ~Draw() = default;
    explicit Draw(HINSTANCE hInstance_);

    static void draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_display);
};

struct Game_Data {
    int N, M; // fields
    int maxtick;
    int tick;

    std::vector<std::vector<std::vector<std::vector<std::string>>>> fd;
};

struct Render_State {
    int height, width; // screen size
    void* memory; // void pointer: nem érdekel minket milyen típusú
    int right_padding;
    int bottom_padding;

    BITMAPINFO bitmap_info;
};

extern Game_Data game_data;
extern Render_State render_state; // muszáj hogy globális legyen, mert csak a WndProc-on belül érhető el a méret módosítás, és az pedig static
extern HDC hdc;
extern HWND main_window;
extern std::vector<std::vector<HWND>> field_btns;

void draw_filled_rect(int x0, int y0, int width, int height, uint32_t color);
void draw_rect(int x0, int y0, int width, int height, uint32_t color, int border_width);
void clear();
void refreshWindowElements();
void refresh();
uint32_t coord(int x, int y);


#endif //ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
