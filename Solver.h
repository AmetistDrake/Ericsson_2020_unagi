#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <unordered_set>

class Solver {
private:
    Reader reader;
    struct Action {
        unsigned int y,x,val;
    };

    std::vector<std::vector<std::vector<Area>>> tick_info;
    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history;
    std::unordered_set<unsigned int> infected_districts;
    std::vector<Action> BACK;
    std::vector<Action> PUT;

    unsigned int healing(unsigned int y, unsigned int x);
    unsigned int infection(unsigned int y, unsigned int x);
    static void update_factor(uint32_t&);
    void update_infected_districts();
    void answer_msg(std::vector<std::string>&);

public:
    Solver();
    ~Solver();
    std::vector<std::string> process(const std::vector<std::string>& infos);
    std::vector<std::vector<std::vector<std::vector<std::string>>>> field_display; // field_info[tick][y][x][message_index]
};


#endif //SOCKET_CONNECTOR_SOLVER_H
