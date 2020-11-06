#ifndef ERICSSON_SOCKET_CONNECTOR_DISPLAYINFO_HPP
#define ERICSSON_SOCKET_CONNECTOR_DISPLAYINFO_HPP

#include <utility>
#include <vector>

class DisplayInfo {
private:
    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history;

public:
    DisplayInfo() = delete;
    ~DisplayInfo() = default;
    explicit DisplayInfo(std::vector<std::vector<std::vector<unsigned int>>> infection_history_,
                         std::vector<std::vector<std::vector<unsigned int>>> healing_history_) :
                         infection_history(std::move(infection_history_)), healing_history(std::move(healing_history_)) {}
};

#endif //ERICSSON_SOCKET_CONNECTOR_DISPLAYINFO_HPP
