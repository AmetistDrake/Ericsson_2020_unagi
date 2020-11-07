#ifndef ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
#define ERICSSON_SOCKET_CONNECTOR_DRAW_HPP

#include "windows.h"
#include <iostream>
#include <vector>
#include <algorithm>

class Draw {
private:
    HINSTANCE hInstance;
    HINSTANCE hPrevInstance;
    LPSTR lpCmdLine;
    int nShowCmd;

    WNDCLASS window_class = {};
    HDC hdc;
    HWND window;

    static LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static uint32_t coord(int x, int y);
public:
    Draw() = delete;
    Draw(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd);
    ~Draw() = default;

    void draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_display);

    void refresh();
    static void clear();
    static void draw_filled_rect(int x0, int y0, int width, int height, uint32_t color);
    static void draw_rect(int x0, int y0, int width, int height, uint32_t color, int border_width);
};


#endif //ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
