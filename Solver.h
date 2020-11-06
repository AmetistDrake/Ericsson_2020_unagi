#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <unordered_set>
#include "DisplayInfo.hpp"


class Solver {
private:
    Reader reader;

    std::vector<std::vector<std::vector<Area>>> tick_info;
    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history;
    std::unordered_set<unsigned int> infected_districts;

    unsigned int healing(unsigned int y, unsigned int x);
    unsigned int infection(unsigned int y, unsigned int x);
    static void update_factor(uint32_t&);
    void update_infected_districts();
    void answer_msg(std::vector<std::string>&);

public:
    Solver();
    ~Solver();
    std::vector<std::string> process(const std::vector<std::string>& infos);
    DisplayInfo get_display_info();
};


#endif //SOCKET_CONNECTOR_SOLVER_H
