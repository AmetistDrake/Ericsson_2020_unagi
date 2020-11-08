#ifndef ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
#define ERICSSON_SOCKET_CONNECTOR_DRAW_HPP

#include <windows.h>
#include <iostream>
#include <vector>
#include <algorithm>

class Draw {
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    Draw() = delete;
    ~Draw() = default;
    explicit Draw(HINSTANCE hInstance_);

    static void draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_display);
};

struct Render_State {
    int height, width;
    void* memory; // void pointer: nem érdekel minket milyen típusú

    BITMAPINFO bitmap_info;
};

extern Render_State render_state; // muszáj hogy globális legyen, mert csak a WndProc-on belül érhető el a méret módosítás, és az pedig static
extern HDC hdc;

void draw_filled_rect(int x0, int y0, int width, int height, uint32_t color);
void refresh();


#endif //ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
