#ifndef ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
#define ERICSSON_SOCKET_CONNECTOR_DRAW_HPP

#include "windows.h"
#include <iostream>
#include <vector>

class Draw {
private:
    HINSTANCE hInstance;
    HINSTANCE hPrevInstance;
    LPSTR lpCmdLine;
    int nShowCmd;

    static LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    Draw() = delete;
    Draw(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd);
    ~Draw() = default;

    void draw(const std::vector<std::vector<std::vector<std::vector<std::string>>>>& field_info);
};


#endif //ERICSSON_SOCKET_CONNECTOR_DRAW_HPP
