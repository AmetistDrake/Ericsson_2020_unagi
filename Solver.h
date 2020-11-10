#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <unordered_set>
#include <fstream>
#include <filesystem>

class Solver {
private:
    Reader reader;
    struct Action {
        unsigned int y,x,val;
    };

    std::vector<std::vector<std::vector<Area>>> tick_info;
    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history;
    std::vector<std::vector<std::vector<unsigned int>>> vaccine_history;
    std::vector<std::vector<std::vector<std::vector<std::string>>>> msg_history;

    std::unordered_set<unsigned int> infected_districts;
    std::vector<Action> BACK;
    std::vector<Action> PUT;

    unsigned int healing(unsigned int y, unsigned int x);
    unsigned int infection(unsigned int y, unsigned int x);
    static void update_factor(uint32_t&);
    void update_infected_districts();
    void cleaned_back(); // 3mas pont a második fordulóban
    void answer_msg(std::vector<std::string>&);
    void vaccine_production();

public:
    Solver();
    ~Solver();
    std::vector<std::string> process(const std::vector<std::string>& infos);
    void create_json_from_data();
};


#endif //SOCKET_CONNECTOR_SOLVER_H
